#include "../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../Include/DynamsoftUtility.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <filesystem>
#include <thread>
#include <set>
#include <map>
#include <fstream>
#include <atomic>
using namespace dynamsoft::cvr;
using namespace dynamsoft::license;
using namespace dynamsoft::basic_structures;
using namespace dynamsoft::dbr;
using namespace dynamsoft::utility;

namespace fs = std::filesystem;

// region: The following code only applies to Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#endif
#endif
// endregion

#define SAMPLE_IMAGE_PATH "../../Images/sample_grid.png"
#ifndef GRID_TEMPLATE_BASE_PATH
#if defined(_WIN32)
#define GRID_TEMPLATE_BASE_PATH "../../CustomTemplates/"
#else
#define GRID_TEMPLATE_BASE_PATH "./"
#endif
#endif

#define LIGHTWEIGHT_TEMPLATE_PATH GRID_TEMPLATE_BASE_PATH "GridFastScan.json"
#define LIGHTWEIGHT_TEMPLATE_NAME "GridFastScan"
#define DEEP_DECODE_TEMPLATE_PATH GRID_TEMPLATE_BASE_PATH "GridDeepDecode.json"
#define DEEP_DECODE_TEMPLATE_NAME "GridDeepDecode"
#define RESULT_DIR "./Result/"

#define SCALE_FACTOR 2.0f

CPoint computeCenter(const CQuadrilateral& quad)
{
	CPoint center{ 0,0 };
	for (const auto& p : quad.points) {
		center[0] += p[0];
		center[1] += p[1];
	}
	center[0] *= (1.0f / 4);
	center[1] *= (1.0f / 4);
	return center;
}

CQuadrilateral expandQuad(const CQuadrilateral& quad, float scale)
{
	CPoint center = computeCenter(quad);
	CQuadrilateral result;
	for (int i = 0; i < 4; ++i)
	{
		result.points[i][0] = center[0] + (quad.points[i][0] - center[0]) * scale;
		result.points[i][1] = center[1] + (quad.points[i][1] - center[1]) * scale;
	}
	return result;
}

enum class LayoutAnalysisAndDecodeTextResultType
{
	LAADTRT_DECODE_FAILED,
	LAADTRT_DECODED,
	LAADTRT_INFERRED
};

struct InputParams
{
	bool exit{ false };
	std::string imagePath{ "" };
};

class CommonDecodeResult
{
public:
	CommonDecodeResult(int err = 0) : error(err) {}
	void prepareForLayoutAnalysis()
	{
		// set id to quad in order to find the corresponding text for layout analysis result.
		for (int i = 0; i < locations.size(); ++i)
		{
			locations[i].id = i;
		}
	}
	std::string getText(int id) const
	{
		if (id < 0 || id >= texts.size())
			throw std::out_of_range("Invalid id.");
		return texts[id];
	}

public:
	std::vector<CQuadrilateral> locations;
	std::vector<std::string> texts;
	int error{ 0 };
private:
	static bool sameQuadrilateral(const CQuadrilateral& quad1, const CQuadrilateral& quad2)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (quad1.points[i][0] != quad2.points[i][0] || quad1.points[i][1] != quad2.points[i][1])
				return false;
		}
		return true;
	}
};

class LayoutAnalysisAndDecodeTextResultItem
{
public:
	LayoutAnalysisAndDecodeTextResultItem(const LayoutElement& element)
	{
		location = element.quad;
		type = LayoutAnalysisAndDecodeTextResultType::LAADTRT_INFERRED;
	}

	LayoutAnalysisAndDecodeTextResultItem(const CQuadrilateral& quad, const std::string& text)
	{
		location = quad;
		this->text = text;
		type = LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODED;
	}

	LayoutAnalysisAndDecodeTextResultItem() = default;

	CQuadrilateral location;
	std::string text;
	LayoutAnalysisAndDecodeTextResultType type{ LayoutAnalysisAndDecodeTextResultType::LAADTRT_INFERRED };
};

class MyLayoutAnalysisResult
{
public:

	MyLayoutAnalysisResult() : layoutResultInner(nullptr) {}
	MyLayoutAnalysisResult(const MyLayoutAnalysisResult& other) = delete;
	MyLayoutAnalysisResult(LayoutAnalysisResult* pResult)
		: layoutResultInner(pResult) {
	}
	~MyLayoutAnalysisResult()
	{
		if (layoutResultInner)
		{
			CLayoutAnalyzer::ReleaseResult(layoutResultInner);
			layoutResultInner = nullptr;
		}
	}

public:
	LayoutAnalysisResult* layoutResultInner;

};

class LayoutAnalysisAndDecodeTextResult
{
public:
	template <typename T>
	using Sparse2DArray = std::unordered_map<int, std::unordered_map<int, T>>;

	LayoutAnalysisAndDecodeTextResult(const CommonDecodeResult& commonResult, const MyLayoutAnalysisResult& layoutResult)
	{
		for (int row_i = 0; row_i < layoutResult.layoutResultInner->rowCount; ++row_i)
		{
			for (int col_i = 0; col_i < layoutResult.layoutResultInner->colCount; ++col_i)
			{
				const LayoutElement& element = layoutResult.layoutResultInner->elements[row_i][col_i];
				switch (element.source)
				{
				case LES_INPUT:
				{
					try
					{
						std::string text = commonResult.getText(element.quad.id);
						LayoutAnalysisAndDecodeTextResultItem item(element.quad, text);
						items[row_i][col_i] = std::move(item);
					}
					catch (const std::exception& e)
					{
						std::cerr << "Error retrieving text for quad id " << element.quad.id << ": " << e.what() << std::endl;
					}
					break;
				}
				case LES_INFERRED:
				{
					LayoutAnalysisAndDecodeTextResultItem item(element);
					items[row_i][col_i] = std::move(item);
					break;
				}
				case LES_NONE:
				default:
					break;
				}
			}
		}
	}

	void updateItem(int row, int col, const std::string& text, const CQuadrilateral& quad)
	{
		std::lock_guard<std::mutex> lock(mtx);

		auto rowIt = items.find(row);
		if (rowIt == items.end())
			throw std::out_of_range("Row " + std::to_string(row) + " not found.");

		auto colIt = rowIt->second.find(col);
		if (colIt == rowIt->second.end())
			throw std::out_of_range("Column " + std::to_string(col) + " not found in row " + std::to_string(row) + ".");

		colIt->second.text = text;
		colIt->second.location = quad;
	}

	void updateItemWithDecodeFailed(int row, int col)
	{
		std::lock_guard<std::mutex> lock(mtx);
		auto rowIt = items.find(row);
		if (rowIt == items.end())
			throw std::out_of_range("Row " + std::to_string(row) + " not found.");

		auto colIt = rowIt->second.find(col);
		if (colIt == rowIt->second.end())
			throw std::out_of_range("Column " + std::to_string(col) + " not found in row " + std::to_string(row) + ".");

		colIt->second.text = "";
		colIt->second.type = LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODE_FAILED;
	}

	std::string toJson() const
	{
		int totalDecoded = 0;
		int totalInferred = 0;
		std::string gridJson = "";

		// Use a map to sort items by row and column for consistent output
		std::map<int, std::map<int, const LayoutAnalysisAndDecodeTextResultItem*>> sortedItems;
		for (const auto& rowPair : items)
		{
			for (const auto& colPair : rowPair.second)
			{
				sortedItems[rowPair.first][colPair.first] = &colPair.second;
			}
		}

		for (auto const& [rowIdx, colMap] : sortedItems)
		{
			for (auto const& [colIdx, itemPtr] : colMap)
			{
				const auto& item = *itemPtr;
				std::string status;
				if (item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODED || item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_INFERRED) {
					status = "Decoded";
					totalDecoded++;
				}
				else if (item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODE_FAILED) {
					status = "Inferred";
					totalInferred++;
				}
				else {
					status = "Failed";
				}

				if (!gridJson.empty()) gridJson += ",";
				gridJson += "\n\t\t{ \"row\": " + std::to_string(rowIdx + 1) +
					", \"col\": " + std::to_string(colIdx + 1) +
					", \"status\": \"" + status +
					"\", \"text\": \"" + escapeJsonString(item.text) + "\" }";
			}
		}

		std::string json = "{\n";
		json += "\t\"totalDecoded\": " + std::to_string(totalDecoded) + ",\n";
		json += "\t\"totalInferred\": " + std::to_string(totalInferred) + ",\n";
		json += "\t\"grid\": [" + gridJson + "\n\t]\n";
		json += "}";
		return json;
	}

private:
	static std::string escapeJsonString(const std::string& input)
	{
		std::string output;
		output.reserve(input.length());
		for (char c : input)
		{
			switch (c)
			{
			case '\"': output += "\\\""; break;
			case '\\': output += "\\\\"; break;
			case '\b': output += "\\b"; break;
			case '\f': output += "\\f"; break;
			case '\n': output += "\\n"; break;
			case '\r': output += "\\r"; break;
			case '\t': output += "\\t"; break;
			default:
				if (static_cast<unsigned char>(c) < 32)
				{
					char buf[10];
					sprintf(buf, "\\u%04x", static_cast<int>(c));
					output += buf;
				}
				else
				{
					output += c;
				}
			}
		}
		return output;
	}
public:
	Sparse2DArray<LayoutAnalysisAndDecodeTextResultItem> items;

private:
	std::mutex mtx;

};

#if defined(_WIN32)
class ImageShower
{
public:
	static ImageShower& instance()
	{
		static ImageShower inst("Grid Barcode Scanner [1/4] Fast Scan", 30);
		return inst;
	}

	ImageShower(const ImageShower&) = delete;
	ImageShower& operator=(const ImageShower&) = delete;

	void update(const cv::Mat& img, const std::string& title)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		img.copyTo(m_img);
		windowTitle = title;
	}

	void stop()
	{
		if (running.exchange(false))
		{
			if (m_thread.joinable())
				m_thread.join();
		}
	}

	void setDelay(int ms)
	{
		delayMs = ms;
	}

private:
	ImageShower(const std::string& windowName, int delayMs)
		: winName(windowName),
		delayMs(delayMs),
		windowTitle(windowName),
		running(true)
	{
		m_thread = std::thread(&ImageShower::loop, this);
	}

	~ImageShower()
	{
		stop();
	}

	void loop()
	{
		cv::namedWindow(winName, cv::WINDOW_NORMAL);
		cv::resizeWindow(winName, cv::Size(800, 600));
		cv::moveWindow(winName, 100, 100);
		std::string lastTitle = windowTitle;
		while (running)
		{
			cv::Mat img;
			std::string title;

			{
				std::lock_guard<std::mutex> lock(m_mutex);
				if (!m_img.empty())
					img = m_img.clone();
				m_img = cv::Mat();
				title = windowTitle;
			}

			if (!img.empty())
			{
				if (title != lastTitle)
				{
					cv::setWindowTitle(winName, title);
					lastTitle = title;
				}
				cv::Rect rect = cv::getWindowImageRect(winName);
				int winW = rect.width;
				int winH = rect.height;
				double scale = std::min(
					(double)winW / img.cols,
					(double)winH / img.rows
				);
				int newW = (int)(img.cols * scale);
				int newH = (int)(img.rows * scale);
				cv::resizeWindow(winName, cv::Size(newW, newH));
				cv::imshow(winName, img);
			}
			else
			{
				int key = cv::waitKey(delayMs);

				if (key == 27)
					running = false;

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			int key = cv::waitKey(delayMs);

			if (key == 27)
				running = false;

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

private:
	std::string winName;
	std::string windowTitle;
	int delayMs;

	std::thread m_thread;
	std::mutex m_mutex;

	cv::Mat m_img;
	std::atomic<bool> running;
};
#else
class ImageShower
{
public:
	static ImageShower& instance()
	{
		static ImageShower inst;
		return inst;
	}

	ImageShower(const ImageShower&) = delete;
	ImageShower& operator=(const ImageShower&) = delete;

	void update(const cv::Mat&, const std::string&) {}
	void stop() {}
	void setDelay(int) {}

private:
	ImageShower() = default;
	~ImageShower() = default;
};
#endif

class ImageHelper
{
public:
	ImageHelper(const std::string& path)
	{
		img = cv::imread(path, cv::IMREAD_COLOR);
		if (img.empty())
			throw std::runtime_error("Failed to read image from file: " + path);
		imagePath = path;
	}

	~ImageHelper() = default;

	int width() const
	{
		return img.cols;
	}

	int height() const
	{
		return img.rows;
	}

	int saveResultImage(CommonDecodeResult* result)
	{
		ImageHelper resultImage = drawSolidQuads(result->locations, cv::Scalar(0, 255, 0, 255), 2);
		ImageShower::instance().update(resultImage.img,"Grid Barcode Scanner [1/4] Fast Scan");
		return resultImage.saveToResultFile("_phase1");
	}

	int saveResultImage(const MyLayoutAnalysisResult& layoutResult)
	{
		std::vector<CQuadrilateral> decodeQuads;
		std::vector<CQuadrilateral> inferredQuads;
		for (int i = 0; i < layoutResult.layoutResultInner->rowCount; ++i)
		{
			for (int j = 0; j < layoutResult.layoutResultInner->colCount; ++j)
			{
				const LayoutElement& element = layoutResult.layoutResultInner->elements[i][j];
				if (element.source == LES_INPUT)
					decodeQuads.push_back(element.quad);
				else if (element.source == LES_INFERRED)
					inferredQuads.push_back(element.quad);
			}
		}
		ImageHelper decodeImage = drawSolidQuads(decodeQuads, cv::Scalar(0, 255, 0, 255), 2);
		ImageHelper inferredImage = decodeImage.drawDashedQuad(inferredQuads, cv::Scalar(0, 0, 255, 255), 10, 4);

		ImageShower::instance().update(inferredImage.img, "Grid Barcode Scanner [2/4] Layout Analysis");
		return inferredImage.saveToResultFile("_phase2");
	}

	int saveResultImage(const LayoutAnalysisAndDecodeTextResult& result)
	{
		std::vector<CQuadrilateral> decodeQuads;
		std::vector<CQuadrilateral> inferredQuads;
		std::vector<CQuadrilateral> undecodedQuads;
		int total{ 0 }, decoded{ 0 };
		for (const auto& row : result.items)
		{
			for (const auto& col : row.second)
			{
				total++;
				const LayoutAnalysisAndDecodeTextResultItem& item = col.second;
				if (item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODED)
				{
					decoded++;
					decodeQuads.push_back(item.location);

				}
				else if (item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_INFERRED)
				{
					decoded++;
					inferredQuads.push_back(item.location);
				}
				else if (item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODE_FAILED)
				{
					undecodedQuads.push_back(expandQuad(item.location, SCALE_FACTOR));
				}
			}
		}
		ImageHelper decodeImage = drawSolidQuads(decodeQuads, cv::Scalar(0, 255, 0, 255), 2);
		ImageHelper inferredImage = decodeImage.drawSolidQuads(inferredQuads, cv::Scalar(0, 255, 0, 255), 2);
		ImageHelper undecodedImage = inferredImage.drawDashedQuad(undecodedQuads, cv::Scalar(0, 0, 255, 255), 10, 4);

		ImageShower::instance().update(undecodedImage.img, "Grid Barcode Scanner [3/4] Deep Decode");
		return undecodedImage.saveToResultFile("_phase3");
	}

	int saveFinalResultImage(const LayoutAnalysisAndDecodeTextResult& result)
	{
		std::vector<CQuadrilateral> decodeQuads;
		std::vector<CQuadrilateral> inferredQuads;
		std::vector<CQuadrilateral> undecodedQuads;
		for (const auto& row : result.items)
		{
			for (const auto& col : row.second)
			{
				const LayoutAnalysisAndDecodeTextResultItem& item = col.second;
				if (item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODED)
				{
					decodeQuads.push_back(item.location);

				}
				else if (item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_INFERRED)
				{
					inferredQuads.push_back(item.location);
				}
				else if (item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODE_FAILED)
				{
					undecodedQuads.push_back(expandQuad(item.location, SCALE_FACTOR));
				}
			}
		}

		ImageHelper decodeImage = drawSolidQuads(decodeQuads, cv::Scalar(0, 255, 0, 255), 2);
		ImageHelper inferredImage = decodeImage.drawSolidQuads(inferredQuads, cv::Scalar(0, 255, 0, 255), 2);
		ImageHelper undecodedImage = inferredImage.drawDashedQuad(undecodedQuads, cv::Scalar(0, 0, 255, 255), 10, 4);

		for (const auto& row : result.items)
		{
			for (const auto& col : row.second)
			{
				const LayoutAnalysisAndDecodeTextResultItem& item = col.second;
				if (item.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODE_FAILED)
					continue;

				cv::Point topLeft = GetTopLeft(item.location);
				undecodedImage.drawText(item.text, topLeft, cv::Scalar(0, 255, 0, 255), 0.6);
			}
		}
		ImageShower::instance().update(undecodedImage.img, "Grid Barcode Scanner [4/4] Final Result");
		return undecodedImage.saveToResultFile("_final");
	}

	std::shared_ptr<CImageData> image() const
	{
		return getCImageData(img);
	}

	static void createResultDir(const std::string& imgPath)
	{
		fs::path dir = RESULT_DIR / fs::path(imgPath).filename().stem();
		fs::create_directories(dir);
	}

	std::string getResultDir() const
	{
		fs::path resultPath = RESULT_DIR / imagePath.filename().stem();
		return resultPath.string();
	}
private:
	ImageHelper(cv::Mat mat, const std::string& path) : img(mat), imagePath(path) {}

	ImageHelper clone()
	{
		cv::Mat mat = img.clone();
		return { mat,imagePath.string() };
	}

	bool saveToResultFile(const std::string& fix) const
	{
		fs::path resultPath = RESULT_DIR / imagePath.stem() / (imagePath.stem().string() + fix + imagePath.extension().string());
		return cv::imwrite(resultPath.string(), img);
	}

	ImageHelper drawSolidQuads(const std::vector<CQuadrilateral>& quads, cv::Scalar color = cv::Scalar(0, 0, 255), int thickness = 2)
	{
		cv::Mat dst = img.clone();
		std::vector<std::vector<cv::Point>> cvQuads;
		for (auto& quad : quads)
		{
			cvQuads.push_back({ cv::Point(quad.points[0][0], quad.points[0][1]),
				cv::Point(quad.points[1][0], quad.points[1][1]),
				cv::Point(quad.points[2][0], quad.points[2][1]),
				cv::Point(quad.points[3][0], quad.points[3][1]) });
		}
		cv::polylines(dst, cvQuads, true, color, thickness, cv::LINE_8);
		return { dst, imagePath.string() };
	}

	ImageHelper drawDashedQuad(const std::vector<CQuadrilateral>& quads, cv::Scalar color, int dashLength = 5, int thickness = 2)
	{
		cv::Mat dst = img.clone();
		auto drawDashedLine = [&](cv::Point p1, cv::Point p2)
			{
				cv::LineIterator it(dst, p1, p2, 8);

				bool draw = true;
				int count = 0;

				for (int i = 0; i < it.count; i++, ++it)
				{
					if (draw)
					{
						cv::circle(
							dst,
							it.pos(),
							thickness / 2,
							cv::Scalar(color[0], color[1], color[2]),
							cv::FILLED,
							cv::LINE_AA
						);
					}

					count++;
					if (count == dashLength)
					{
						count = 0;
						draw = !draw;
					}
				}
			};

		for (auto& quad : quads)
		{
			for (int i = 0; i < 4; i++)
			{
				cv::Point p1(quad.points[i][0], quad.points[i][1]);
				cv::Point p2(quad.points[(i + 1) % 4][0], quad.points[(i + 1) % 4][1]);
				drawDashedLine(p1, p2);
			}
		}
		return { dst, imagePath.string() };
	}

	static cv::Point GetTopLeft(const CQuadrilateral& quad)
	{
		int idx = 0;
		cv::Point topLeft(quad.points[0][0], quad.points[0][1]);
		for (int i = 1; i < 4; i++)
		{
			if (quad.points[i][0] < topLeft.x)
			{
				topLeft.x = quad.points[i][0];
			}
			if (quad.points[i][1] < topLeft.y)
			{
				topLeft.y = quad.points[i][1];
			}
		}
		return topLeft;
	}

	void drawText(const std::string& text, cv::Point org, cv::Scalar color, double scale = 1.0)
	{
		cv::putText(img, text, org,
			cv::FONT_HERSHEY_SIMPLEX,
			scale,
			color,
			2,
			cv::LINE_AA);
	}

	static std::shared_ptr<CImageData> getCImageData(const cv::Mat& mat)
	{
		cv::Mat src = mat;

		if (!src.isContinuous())
			src = src.clone();

		unsigned long long bytesLength = src.total() * src.elemSize();

		const unsigned char* bytes = src.data;

		int width = src.cols;
		int height = src.rows;
		int stride = static_cast<int>(src.step);

		return std::make_shared<CImageData>(
			bytesLength,
			bytes,
			width,
			height,
			stride,
			ImagePixelFormat::IPF_BGR_888
		);
	}
private:
	cv::Mat img;
	fs::path imagePath;
};

struct Task {
	int row;
	int col;
	LayoutAnalysisAndDecodeTextResultItem* item;
};

InputParams welcome()
{
	InputParams params;
	std::cout << "Grid Barcode Scanner!" << std::endl
		<< "===========================" << std::endl;
	std::cout << std::endl << "Image path : [press Enter to use sample image (sample_grid.png)]" << std::endl
		<< "'Q'/'q' to quit" << std::endl;
	std::string input;
	std::getline(std::cin, input);
	if (input == "Q" || input == "q")
	{
		params.exit = true;
		return params;
	}
	if (input.empty())
		params.imagePath = SAMPLE_IMAGE_PATH;
	else
	{
		if (input.length() >= 2 && input.front() == '"' && input.back() == '"')
			input = input.substr(1, input.length() - 2);
		params.imagePath = input;
	}

	return params;
}

CommonDecodeResult commonDecode(const ImageHelper& image, const std::string& templatePath, const std::string& templateName = std::string())
{
	CCaptureVisionRouter cvRouter;
	char error[512];
	int errorCode = cvRouter.InitSettingsFromFile(templatePath.c_str(), error, 512);
	if (errorCode != ErrorCode::EC_OK)
	{
		std::cout << "Failed to initialize settings from file: ErrorCode: " << errorCode << ", ErrorString: " << error << std::endl;
		return { errorCode };
	}
	auto start = std::chrono::high_resolution_clock::now();
	CCapturedResult* result = cvRouter.Capture(image.image().get(), templateName.c_str());
	auto end = std::chrono::high_resolution_clock::now();
	if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
	{
		std::cout << "Common decode warning: " << result->GetErrorCode() << ", " << result->GetErrorString() << std::endl;
	}
	else if (result->GetErrorCode() != ErrorCode::EC_OK && result->GetErrorCode() != ErrorCode::EC_TIMEOUT)
	{
		std::cout << "Common decode error: " << result->GetErrorCode() << "," << result->GetErrorString() << std::endl;
		int ret = result->GetErrorCode();
		result->Release();
		return { ret };
	}
	CommonDecodeResult decodeResult;
	for (int i = 0; i < result->GetItemsCount(); ++i)
	{
		const CCapturedResultItem* item = result->GetItem(i);
		if (item && (item->GetType() == CapturedResultItemType::CRIT_BARCODE))
		{
			const CBarcodeResultItem* barcodeItem = static_cast<const CBarcodeResultItem*>(item);
			decodeResult.locations.push_back(barcodeItem->GetLocation());
			decodeResult.texts.push_back(barcodeItem->GetText());
		}
	}
	std::cout << "[Phase 1] Fast scan: " << decodeResult.texts.size() << " barcodes decoded in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms." << std::endl;
	result->Release();
	return decodeResult;
}
int getMedianFuncValue(const std::vector<CQuadrilateral>& vec)
{
	if (vec.empty())
		return 0;

	std::vector<int> values;
	values.reserve(vec.size());

	for (const auto& quad : vec)
	{
		values.push_back(quad.GetArea());
	}

	std::sort(values.begin(), values.end());

	size_t n = values.size();

	if (n % 2 == 1)
	{
		return values[n / 2];
	}
	else
	{
		return (values[n / 2 - 1] + values[n / 2]) / 2;
	}
}
int findDifferentLengthIndex(const std::vector<std::string>&vec)
{
	std::unordered_map<size_t, int> lenCount;

	for (const auto& s : vec)
	{
		lenCount[s.size()]++;
	}

	if (lenCount.size() != 2)
	{
		return -1;
	}

	size_t abnormalLen = 0;
	int abnormalTypeCount = 0;

	for (const auto& [len, count] : lenCount)
	{
		if (count == 1)
		{
			abnormalLen = len;
			abnormalTypeCount++;
		}
	}

	if (abnormalTypeCount != 1)
	{
		return -1;
	}

	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i].size() == abnormalLen)
		{
			return i;
		}
	}

	return -1;
}

MyLayoutAnalysisResult Analyze(const ImageHelper& imageHelper, const CommonDecodeResult& result)
{
	LayoutAnalysisParameter layoutParam;
	layoutParam.pattern = LayoutPattern::LP_MATRIX;
	layoutParam.inputImageHeight = imageHelper.height();
	layoutParam.inputImageWidth = imageHelper.width();

	LayoutAnalysisResult* layoutResult = CLayoutAnalyzer::Analyze(result.locations.data(), result.locations.size(), &layoutParam);
	if (!layoutResult || layoutResult->errorCode != ErrorCode::EC_OK)
	{
		std::cout << "Layout analysis failed: ErrorCode: " << (layoutResult ? layoutResult->errorCode : -1) << std::endl;
		if (layoutResult)
			CLayoutAnalyzer::ReleaseResult(layoutResult);
		return { };
	}
	std::cout << std::endl << "[Phase 2] Layout analysis";
	std::cout << ": "
		<< layoutResult->colCount * layoutResult->rowCount << " grid positions (" << layoutResult->rowCount << "x" << layoutResult->colCount << "). "
		<< layoutResult->inferredQuadCount << " inferred regions.";
	std::cout << std::endl;
	return { layoutResult };
}

void deepDecodeInner(
	const ImageHelper& image,
	const std::string& templatePath,
	LayoutAnalysisAndDecodeTextResult* laadtResult,
	const Task& task,
	const std::string& templateName = std::string())
{
	CCaptureVisionRouter cvRouter;
	char error[512];
	int errorCode = cvRouter.InitSettingsFromFile(templatePath.c_str(), error, 512);
	if (errorCode != ErrorCode::EC_OK)
	{
		laadtResult->updateItemWithDecodeFailed(task.row, task.col);
		return;
	}

	SimplifiedCaptureVisionSettings settings;

	errorCode = cvRouter.GetSimplifiedSettings(templateName.c_str(), &settings);

	if(errorCode != ErrorCode::EC_OK)
	{
		laadtResult->updateItemWithDecodeFailed(task.row, task.col);
		return;
	}

	settings.roi = expandQuad(task.item->location, SCALE_FACTOR);
	settings.roiMeasuredInPercentage = 0;
	errorCode = cvRouter.UpdateSettings(templateName.c_str(), &settings, error, 512);

	if (errorCode != ErrorCode::EC_OK)
	{
		laadtResult->updateItemWithDecodeFailed(task.row, task.col);
		return;
	}

	CCapturedResult* result = cvRouter.Capture(image.image().get(), templateName.c_str());

	if (result->GetErrorCode() != ErrorCode::EC_OK)
	{
		laadtResult->updateItemWithDecodeFailed(task.row, task.col);
		result->Release();
		return;
	}

	for (int i = 0; i < result->GetItemsCount(); ++i)
	{
		const CCapturedResultItem* item = result->GetItem(i);
		if (item && item->GetType() == CapturedResultItemType::CRIT_BARCODE)
		{
			const CBarcodeResultItem* barcodeItem = static_cast<const CBarcodeResultItem*>(item);
			const char* text = barcodeItem->GetText();
			if(text)
				laadtResult->updateItem(task.row, task.col,text, barcodeItem->GetLocation());
			else
				laadtResult->updateItemWithDecodeFailed(task.row, task.col);
			result->Release();
			return;
		}
	}
	laadtResult->updateItemWithDecodeFailed(task.row, task.col);
	result->Release();
}

void deepDecode(const ImageHelper& image, const std::string& templatePath, LayoutAnalysisAndDecodeTextResult* laadtResult, const std::string& templateName = std::string())
{
	auto start = std::chrono::high_resolution_clock::now();
	std::vector<Task> tasks;
	for(auto &row:laadtResult->items)
	{
		for(auto &col:row.second)
		{
			if(col.second.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_INFERRED)
			{
				tasks.push_back({ row.first, col.first, &col.second });
			}
		}
	}
	const size_t n = tasks.size();
	if (n == 0) return;

	unsigned numThreads = std::thread::hardware_concurrency();
	if (numThreads == 0) numThreads = 4;
	std::atomic<size_t> index{ 0 };
	std::vector<std::thread> threads;
	threads.reserve(numThreads);
	for (unsigned t = 0; t < numThreads; ++t)
	{
		threads.emplace_back([&]() {
			while (true)
			{
				size_t i = index.fetch_add(1, std::memory_order_relaxed);
				if (i >= n) break;

				auto& task = tasks[i];
				try
				{
					deepDecodeInner(image, templatePath, laadtResult, task, templateName);
				}
				catch (const std::exception& e)
				{
					std::cerr << "Error in deep decode for row " << task.row << ", col " << task.col << ": " << e.what() << std::endl;
				}
			}
			});
	}

	for (auto& th : threads) {
		th.join();
	}
	auto end = std::chrono::high_resolution_clock::now();
	int decodeCount = tasks.size();
	for(auto &row:laadtResult->items)
	{
		for(auto &col:row.second)
		{
			if(col.second.type == LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODE_FAILED)
			{
				decodeCount--;
			}
		}
	}
	std::cout << std::endl << "[Phase 3] Deep decode: " << decodeCount << " / " << tasks.size() << " inferred regions decoded in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms." << std::endl;
}

void printResult(const LayoutAnalysisAndDecodeTextResult& laadtResult, const std::string& path)
{
	std::cout << "============================================================" << std::endl;
	std::cout << " " << std::left << std::setw(4) << "Row"
		<< " | " << std::setw(5) << "Col"
		<< " | " << std::setw(13) << "Status"
		<< " | " << "Text" << std::endl;
	std::cout << "============================================================" << std::endl;
	int total = 0, decoded = 0;
	for (auto& row : laadtResult.items)
	{
		for (auto& col : row.second)
		{
			total++;
			std::string status;
			switch (col.second.type)
			{
			case LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODE_FAILED:
				status = "Inferred";
				break;
			case LayoutAnalysisAndDecodeTextResultType::LAADTRT_DECODED:
			case LayoutAnalysisAndDecodeTextResultType::LAADTRT_INFERRED:
				status = "Decoded";
				decoded++;
				break;
			default:
				status = "Unknown";
				break;
			}
			std::cout << "  " << std::left << std::setw(3) << row.first + 1
				<< " | " << std::setw(5) << col.first + 1
				<< " | " << std::setw(13) << status
				<< " | " << col.second.text << std::endl;
		}
	}
	std::cout << "============================================================" << std::endl;
	std::cout << "[Done] Total decoded: " << decoded << " / " << total << "." << std::endl;
	std::ofstream ofs(path);
	if (ofs.is_open())
	{
		ofs << laadtResult.toJson();
		ofs.close();
		std::cout << "Detailed results saved to result.json." << std::endl;
	}
	else
	{
		std::cerr << "Failed to save detailed results to result.json." << std::endl;
	}
}

int main()
{
	int errorCode = 0;
	char error[512];

	// 1.Initialize license.
	// You can request and extend a trial license from https://www.dynamsoft.com/customer/license/trialLicense?product=dcv&utm_source=samples&package=c_cpp
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", error, 512);

	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_WARNING)
	{
		std::cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << error << std::endl;
		return 0;
	}
	else
	{
		while (1)
		{
			try
			{
				std::string temp;
				// input decode parameters or exit
				InputParams params = welcome();
				if (params.exit)
					break;

				ImageHelper imageHelper(params.imagePath);
				ImageHelper::createResultDir(params.imagePath);
				// fast scan with lightweight template
				CommonDecodeResult commonDecodeResult = commonDecode(imageHelper, LIGHTWEIGHT_TEMPLATE_PATH, LIGHTWEIGHT_TEMPLATE_NAME);
				if (commonDecodeResult.error != ErrorCode::EC_OK)
				{
					continue;
				}
				commonDecodeResult.prepareForLayoutAnalysis();
				int ret = imageHelper.saveResultImage(&commonDecodeResult);

				std::cout << " Press Enter for Layout Analysis...";
				std::getline(std::cin, temp);


				// layout analysis
				MyLayoutAnalysisResult layoutResult = Analyze(imageHelper, commonDecodeResult);
				if (!layoutResult.layoutResultInner)
					continue;
				LayoutAnalysisAndDecodeTextResult laadtResult(commonDecodeResult, layoutResult);
				imageHelper.saveResultImage(layoutResult);

				std::cout << " Press Enter for Deep Decode...";
				std::getline(std::cin, temp);

				// deep decode with deep decode template
				deepDecode(imageHelper, DEEP_DECODE_TEMPLATE_PATH, &laadtResult, DEEP_DECODE_TEMPLATE_NAME);
				ret = imageHelper.saveResultImage(laadtResult);
				std::cout << " Press Enter to view final result...";
				std::getline(std::cin, temp);

				//print final result
				std::string path = imageHelper.getResultDir().append("/result.json");
				printResult(laadtResult, path);
				ret = imageHelper.saveFinalResultImage(laadtResult);

				std::cout << "Press Enter for next image (or 'Q'/'q' to quit)..." << std::endl;
				std::getline(std::cin, temp);
				if (temp == "Q" || temp == "q")
					break;
			}
			catch (const std::exception& ex)
			{
				std::cout << "An error occurred: " << ex.what() << std::endl;
				break;
			}
		}
	}
}