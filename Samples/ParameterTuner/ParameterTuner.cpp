#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <ctime>
#include "../../Include/DynamsoftCaptureVisionRouter.h"

using namespace dynamsoft::cvr;
using namespace dynamsoft::license;
using namespace dynamsoft::dbr;

#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#endif
#endif

class LoopInfo {
public:
	CCaptureVisionRouter* cvrInstance;
	std::string imagePath;
	std::string templatePath;
	std::string matchedTemplate;
	std::string description;

	LoopInfo(CCaptureVisionRouter* cvr) : cvrInstance(cvr) {}
};

std::tuple<std::string, std::string, std::string> selectImage() {
	std::map<std::string, std::tuple<std::string, std::string, std::string>> sampleImages = {
		{"1", {"images/blurry.png", "ReadBlurry1DBarcode.json", "Suitable for blurred 1D barcode"}},
		{"2", {"images/GeneralBarcodes.png", "ReadMultipleBarcode.json", "Suitable for multiple barcodes"}},
		{"3", {"images/inverted-barcode.png", "ReadInvertedBarcode.json", "Suitable for colour inverted barcode"}},
		{"4", {"images/DPM.png", "ReadDPM.json", "Suitable for Direct Part Marking barcode"}},
		{"5", {"images/EAN-13.jpg", "ReadOneDRetail.json", "Suitable for retail 1D barcode such as EAN13, UPC-A"}},
		{"6", {"images/OneDIndustrial.jpg", "ReadOneDIndustrial.json", "Suitable for industrial 1D barcode such as Code128, Code39"}}
	};

	std::cout << "Available Sample Scenarios:" << std::endl;
	std::cout << "[1] Blurry 1D barcode" << std::endl;
	std::cout << "[2] Multiple Barcodes" << std::endl;
	std::cout << "[3] Colour Inverted Barcode" << std::endl;
	std::cout << "[4] Direct Part Marking (DPM)" << std::endl;
	std::cout << "[5] Retail 1D barcode" << std::endl;
	std::cout << "[6] Industrial 1D barcode" << std::endl;
	std::cout << "[7] Custom Image" << std::endl;

	while (true) {
		std::cout << "\nEnter the number of the image to test, or provide a full path to your own image:" << std::endl;
		std::string choice;
		std::getline(std::cin, choice);

		auto it = sampleImages.find(choice);
		if (it != sampleImages.end()) {
			auto value = it->second;

			std::string imagePath = std::get<0>(value);
			std::string matchedTemplate = std::get<1>(value);
			std::string description = std::get<2>(value);
			imagePath.insert(0, "../../");
			return std::make_tuple(imagePath, matchedTemplate, description);
		}
		else if (choice == "7") {
			std::cout << "Enter full path to your own image:\n> ";
			std::string customPath;
			std::getline(std::cin, customPath);
			if (customPath.length() >= 2 && customPath.front() == '"' && customPath.back() == '"')
				customPath = customPath.substr(1, customPath.length() - 2);

			if (std::ifstream(customPath).good()) {
				return { customPath, "", "" };
			}
		}
		else
		{
			if (choice.length() >= 2 && choice.front() == '"' && choice.back() == '"')
				choice = choice.substr(1, choice.length() - 2);

			if (std::ifstream(choice).good()) {
				return{ choice, "", "" };
			}
		}
		std::cout << "Invalid path input, please try again." << std::endl;
	}
}

std::string selectTemplate(const std::string& matchedTemplate, const std::string& description) {
	std::vector<std::pair<std::string, std::string>> options;

	if (!matchedTemplate.empty()) {
		options.emplace_back(matchedTemplate, description);
	}
	options.emplace_back("ReadBarcodes_Default.json", "General purpose settings");
	options.emplace_back("Custom template", "Use your own template");

	std::cout << "\nSelect template for this image:" << std::endl;
	for (size_t i = 0; i < options.size(); ++i) {
		std::cout << "[" << i + 1 << "] " << options[i].first << " (" << options[i].second << ")" << std::endl;
	}
	std::cout << "\nEnter the number of the template to test, or provide a full path to your own template:" << std::endl;

	while (true) {
		std::string choice;
		std::getline(std::cin, choice);

		if (choice.size()==1 && isdigit(choice[0])) {
			int index = std::stoi(choice) - 1;
			if (index >= 0 && index < options.size()) {
				std::string selectedPath = options[index].first;
				if (index == options.size() - 1) {
					std::cout << "Enter full path to your custom template:\n> ";
					std::getline(std::cin, selectedPath);
				}
				else if (index == options.size() - 2)
				{
					selectedPath = "../../Dist/Templates/DBR-PresetTemplates.json";
				}
				else
				{
					selectedPath.insert(0, "../../CustomTemplates/");
				}

				if (selectedPath.length() >= 2 && selectedPath.front() == '"' && selectedPath.back() == '"')
					selectedPath = selectedPath.substr(1, selectedPath.length() - 2);

				if (std::ifstream(selectedPath).good()) {
					return selectedPath;
				}
				else {
					std::cout << "Invalid template path, please try again." << std::endl;
				}
			}
			else {
				std::cout << "Invalid choice, please try again." << std::endl;
			}
		}
		else if (std::ifstream(choice).good()) {
			return choice;
		}
		else {
			std::cout << "Invalid input, please try again." << std::endl;
		}
	}
}

void run(CCaptureVisionRouter* cvrInstance, const std::string& imagePath, const std::string& templatePath) {
	std::cout << "\nRunning with:" << std::endl;
	std::cout << "Image: " << imagePath << std::endl;
	std::cout << "Template: " << templatePath << std::endl;

	std::clock_t startTime = std::clock();
	CCapturedResultArray* resultArray = cvrInstance->CaptureMultiPages(imagePath.c_str(), "");
	int elapsedMs = static_cast<int>((std::clock() - startTime) * 1000 / CLOCKS_PER_SEC);
	int count = resultArray->GetResultsCount();
	for (int i = 0; i < count; ++i)
	{
		std::cout << "Result" << (count > 1 ? " Page-" + std::to_string(i + 1) : "") << ":" << std::endl;
		const CCapturedResult* result = resultArray->GetResult(i);
		if (!result)
			continue;

		int errorCode = result->GetErrorCode();
		if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING) {
			std::cout << "Error: " << errorCode << ", " << result->GetErrorString() << std::endl;
			continue;
		}

		CDecodedBarcodesResult* barcodeResult = result->GetDecodedBarcodesResult();
		if (barcodeResult) {
			int count = barcodeResult->GetItemsCount();
			for (int j = 0; j < count; ++j) {
				const CBarcodeResultItem* item = barcodeResult->GetItem(j);
				std::cout << "Barcode result [" << j << "]: " << item->GetText()
					<< " (Format: " << item->GetFormatString() << ")" << std::endl;
			}
		}
		else
		{
			std::cout << "No barcode decode." << std::endl;
		}
		std::cout << "Time used: " << elapsedMs << " ms...\n" << std::endl;
		if (barcodeResult)
			barcodeResult->Release();
	}
	if (resultArray)
		resultArray->Release();
}

void loopInner(LoopInfo& loopInfo, bool getImage = true, bool getTemplate = true) {
	if (getImage) {
		std::tuple<std::string, std::string, std::string> tuple = selectImage();
		loopInfo.imagePath = std::get<0>(tuple);
		loopInfo.matchedTemplate = std::get<1>(tuple);
		loopInfo.description = std::get<2>(tuple);
	}
	if (getTemplate) {
		loopInfo.templatePath = selectTemplate(loopInfo.matchedTemplate, loopInfo.description);
		char errorString[1024]{ 0 };
		int errorCode = loopInfo.cvrInstance->InitSettingsFromFile(loopInfo.templatePath.c_str(), errorString, 1024);
		if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING) {
			std::cout << "Init template failed: " << errorCode << ", " << errorString << std::endl;
			return;
		}
	}

	run(loopInfo.cvrInstance, loopInfo.imagePath, loopInfo.templatePath);
}

int main() {

	std::cout << "Welcome to ParameterTuner!" << std::endl;

	char errorString[1024]{ 0 };
	int errorCode{ 0 };

	// Initialize license.
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	// You can also request a 30-day trial license in the customer portal: https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", errorString, 1024);
	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_WARNING)
		std::cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << errorString << std::endl;

	CCaptureVisionRouter* cvRouter = new CCaptureVisionRouter();
	LoopInfo loopInfo(cvRouter);
	loopInner(loopInfo);

	while (true) {
		std::cout << "What would you like to do next?" << std::endl;
		std::cout << "[1] Try a different template" << std::endl;
		std::cout << "[2] Load another image" << std::endl;
		std::cout << "[3] Exit" << std::endl;
		std::cout << "Enter your choice:\n> ";

		std::string choice;
		std::getline(std::cin, choice);

		if (choice == "1") {
			loopInner(loopInfo, false, true);
		}
		else if (choice == "2") {
			loopInner(loopInfo, true, false);
		}
		else if (choice == "3") {
			std::cout << "Thank you for using ParameterTuner!" << std::endl;
			break;
		}
		else {
			std::cout << "Invalid input. Returning to main menu." << std::endl;
		}
	}

	delete cvRouter, cvRouter = nullptr;
	return 0;
}