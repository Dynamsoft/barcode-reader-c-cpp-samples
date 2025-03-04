#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include "../../Include/DynamsoftCaptureVisionRouter.h"

using namespace std;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;
using namespace dynamsoft::license;

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <io.h>

#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#endif

#else
#include <string.h>
#include <dirent.h>

#endif

std::mutex coutMutex;

void ThreadDecodeFile(const char *filePath)
{
	CCaptureVisionRouter router;

	CCapturedResult *result = router.Capture(filePath, CPresetTemplate::PT_READ_BARCODES);

	std::lock_guard<std::mutex> lock(coutMutex);

	cout << "Thread: " << this_thread::get_id() << endl;
	cout << "File Name: " << filePath << endl;
	cout << "Error: " << result->GetErrorCode() << ", " << result->GetErrorString() << endl;

	CDecodedBarcodesResult *barcodeResult = result->GetDecodedBarcodesResult();

	if (barcodeResult == nullptr || barcodeResult->GetItemsCount() == 0)
	{
		cout << "No barcode found." << endl;
	}
	else
	{
		cout << "Total barcode(s) found: " << barcodeResult->GetItemsCount() << endl;
		for (int index = 0; index < barcodeResult->GetItemsCount(); ++index)
		{
			const CBarcodeResultItem *barcode = barcodeResult->GetItem(index);
			cout << index + 1 << ": " << barcode->GetFormatString() << ", " << barcode->GetText() << endl;
		}

		barcodeResult->Release();
	}
	if (result != nullptr)
		result->Release();

	cout << endl;
}

void MultiThreadDecoding(const vector<string> &fileList)
{
	const int THREAD_COUNT = 4;

	const std::size_t filesPerThread = fileList.size() / THREAD_COUNT;

	std::vector<std::thread> threads;

	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		std::size_t startIdx = i * filesPerThread;
		std::size_t endIdx = (i == THREAD_COUNT - 1) ? fileList.size() : (startIdx + filesPerThread);

		threads.emplace_back([&fileList, startIdx, endIdx]()
							 {
			for (std::size_t j = startIdx; j < endIdx; ++j) {
				ThreadDecodeFile(fileList[j].c_str());
			} });
	}

	for (auto &thread : threads)
	{
		thread.join();
	}
}

#if defined(_WIN64) || defined(_WIN32)

void GetFiles(const string &imagePath, vector<string> &files)
{
	intptr_t hFile = 0;
	struct _finddata_t fileinfo;
	size_t len = strlen(imagePath.c_str());

	string seachPath = imagePath + "\\*";

	if ((hFile = _findfirst(seachPath.c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
			}
			else
			{
				string strFilePath = imagePath + "\\" + fileinfo.name;
				files.push_back(strFilePath);
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

#else

void GetFiles(const string &imagePath, vector<string> &files)
{
	DIR *dir;
	struct dirent *ptr;
	if ((dir = opendir(imagePath.c_str())) == NULL)
	{
		return;
	}

	while ((ptr = readdir(dir)) != NULL)
	{
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
			continue;
		if (ptr->d_type == 8)
		{
			string strFilePath = imagePath + "/" + ptr->d_name;

			files.push_back(strFilePath);
		}
	}
}

#endif

int main(int argc, const char *argv[])
{
	int errorCode = 0;
	char szErrorMsg[256];

	cout << "***************************************************************" << endl;
	cout << "Welcome to Dynamsoft Barcode Reader MultiThreadDecoding Sample" << endl;
	cout << "***************************************************************" << endl;
	cout << "Hints: Please input 'Q' or 'q' to quit the application." << endl;

	// 1.Initialize license.
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	// You can also request a 30-day trial license in the customer portal: https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", szErrorMsg, 256);
	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_CACHE_USED)
	{
		cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << szErrorMsg << endl;
	}
	else
	{
		while (1)
		{
			cout << ">> Input your image folder's full path:" << endl;

			string imagePath;
			getline(cin, imagePath);

			if (imagePath.empty())
			{
				cout << "Empty input, please try again." << endl;
				continue;
			}

			if (imagePath == "q" || imagePath == "Q")
				break;

			vector<string> vecFiles;
			GetFiles(imagePath, vecFiles);

			if (vecFiles.empty())
			{
				cout << "No image files found, please try again." << endl;
				continue;
			}

			MultiThreadDecoding(vecFiles);

			cout << "Finish." << endl;
		}
	}
	return 0;
}