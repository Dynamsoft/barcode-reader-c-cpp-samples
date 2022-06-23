#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

#include "../../../include/DynamsoftBarcodeReader.h"
#include "../../../include/DynamsoftCommon.h"

using namespace dynamsoft::dbr;

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <io.h>

#ifdef _WIN64
#pragma comment(lib, "../../../lib/Windows/x64/DBRx64.lib")
#else
#pragma comment(lib, "../../../lib/Windows/x86/DBRx86.lib")
#endif

#else
#include <string.h>
#include <dirent.h>

#endif


typedef struct tagMultiThreadDecodeFileInfo
{
	vector<string> vecFiles;
	int iCurrentIndex;
	mutex* mutexInfo;
}MultiThreadDecodeFileInfo;

void ThreadDecodeFile(void* pInfo)
{
	MultiThreadDecodeFileInfo *pMultiThreadDecodeFileInfo = (MultiThreadDecodeFileInfo*)pInfo;
	while (1)
	{
		pMultiThreadDecodeFileInfo->mutexInfo->lock();

		int iImageIndex = pMultiThreadDecodeFileInfo->iCurrentIndex;
		if(iImageIndex >= pMultiThreadDecodeFileInfo->vecFiles.size())
		{
			pMultiThreadDecodeFileInfo->mutexInfo->unlock();
			return;
		}

		string strImageFile = pMultiThreadDecodeFileInfo->vecFiles[iImageIndex];
		pMultiThreadDecodeFileInfo->iCurrentIndex++;
		
		pMultiThreadDecodeFileInfo->mutexInfo->unlock();


		CBarcodeReader reader;
		
		char errorMessage[256];
		//Best coverage settings
		reader.InitRuntimeSettingsWithString("{\"ImageParameter\":{\"Name\":\"BestCoverage\",\"DeblurLevel\":9,\"ExpectedBarcodesCount\":512,\"ScaleDownThreshold\":100000,\"LocalizationModes\":[{\"Mode\":\"LM_CONNECTED_BLOCKS\"},{\"Mode\":\"LM_SCAN_DIRECTLY\"},{\"Mode\":\"LM_STATISTICS\"},{\"Mode\":\"LM_LINES\"},{\"Mode\":\"LM_STATISTICS_MARKS\"}],\"GrayscaleTransformationModes\":[{\"Mode\":\"GTM_ORIGINAL\"},{\"Mode\":\"GTM_INVERTED\"}]}}", CM_OVERWRITE, errorMessage, 256);		
		//Best speend settings
		//reader.InitRuntimeSettingsWithString(temphBarcode, "{\"ImageParameter\":{\"Name\":\"BestSpeed\",\"DeblurLevel\":3,\"ExpectedBarcodesCount\":512,\"LocalizationModes\":[{\"Mode\":\"LM_SCAN_DIRECTLY\"}],\"TextFilterModes\":[{\"MinImageDimension\":262144,\"Mode\":\"TFM_GENERAL_CONTOUR\"}]}}", CM_OVERWRITE, errorMessage, 256);
		//Balance settings
		//reader.InitRuntimeSettingsWithString(temphBarcode, "{\"ImageParameter\":{\"Name\":\"Balance\",\"DeblurLevel\":5,\"ExpectedBarcodesCount\":512,\"LocalizationModes\":[{\"Mode\":\"LM_CONNECTED_BLOCKS\"},{\"Mode\":\"LM_STATISTICS\"}]}}", CM_OVERWRITE, errorMessage, 256);
		
		int iRet = reader.DecodeFile(strImageFile.c_str());

		cout << "Thread: " << this_thread::get_id() << " File Name:" << strImageFile << endl;

		if (iRet != DBR_OK && iRet != DBRERR_MAXICODE_LICENSE_INVALID && iRet != DBRERR_GS1_COMPOSITE_LICENSE_INVALID && iRet != DBRERR_AZTEC_LICENSE_INVALID && iRet != DBRERR_LICENSE_EXPIRED && iRet != DBRERR_QR_LICENSE_INVALID &&
			iRet != DBRERR_1D_LICENSE_INVALID && iRet != DBRERR_PDF417_LICENSE_INVALID && iRet != DBRERR_DATAMATRIX_LICENSE_INVALID && iRet != DBRERR_GS1_DATABAR_LICENSE_INVALID && iRet != DBRERR_PATCHCODE_LICENSE_INVALID &&
			iRet != DBRERR_POSTALCODE_LICENSE_INVALID && iRet != DBRERR_DOTCODE_LICENSE_INVALID && iRet != DBRERR_DPM_LICENSE_INVALID && iRet != DBRERR_IRT_LICENSE_INVALID && iRet != DMERR_NO_LICENSE && iRet != DMERR_TRIAL_LICENSE)
		{
			cout << "Failed to read barcode: " << CBarcodeReader::GetErrorString(iRet) << endl;;
		}
		else
		{
			TextResultArray *paryResult = NULL;
			reader.GetAllTextResults(&paryResult);
			
			if (paryResult->resultsCount == 0)
			{
				cout << "No barcode found." << endl;
			}
			else 
			{
				cout << "Total barcode(s) found: " << paryResult->resultsCount << endl;
			}

			CBarcodeReader::FreeTextResults(&paryResult);
		}

		cout << endl;
	}
}

void MultiThreadDecodeFileFolder(const vector<string>& files)
{
	const int THREAD_COUNT = 4;

	MultiThreadDecodeFileInfo threadDecodeFileInfo;
	
	mutex* mutexInfo = new mutex();
	threadDecodeFileInfo.iCurrentIndex = 0;
	threadDecodeFileInfo.vecFiles = files;
	threadDecodeFileInfo.mutexInfo = mutexInfo;

	thread threads[THREAD_COUNT];
	for (int i = 0; i < THREAD_COUNT; ++i)
		threads[i] = thread(ThreadDecodeFile, &threadDecodeFileInfo);

	for (auto& th : threads) 
		th.join();

	delete mutexInfo, mutexInfo = NULL;
}

#if defined(_WIN64) || defined(_WIN32)

void GetFiles(const string& imagePath, vector<string>& files)
{
	intptr_t hFile = 0;
	struct _finddata_t fileinfo;
	size_t len = strlen(imagePath.c_str());

	string seachPath = imagePath + "\\*";

	if((hFile = _findfirst(seachPath.c_str(), &fileinfo))!=-1)
	{
		do
		{
			if((fileinfo.attrib & _A_SUBDIR))
			{
			}
			else
			{
				string strFilePath = imagePath + "\\" + fileinfo.name;
				files.push_back(strFilePath);
			}
		}while(_findnext(hFile,&fileinfo)==0);

		_findclose(hFile);
	}

}

#else

void GetFiles(const string& imagePath, vector<string>& files)
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

int main(int argc, const char* argv[])
{
	int errorCode = 0;
	char szErrorMsg[256];

	cout << "*************************************************" << endl;
	cout << "Welcome to Dynamsoft Barcode Reader Demo" << endl;
	cout << "*************************************************" << endl;
	cout << "Hints: Please input 'Q'or 'q' to quit the application." << endl;

	// 1.Initialize license.
	// The string "DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9" here is a free public trial license. Note that network connection is required for this license to work.
	// If you don't have a license yet, you can request a trial from https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp 
	errorCode = CBarcodeReader::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", szErrorMsg, 256);
	if (errorCode == DBR_OK)
	{
		cout << "InitLicense Success." << endl;
	}
	else
	{
		cout << szErrorMsg << endl;
	}

	while (1)
	{
		cout << ">> Input your image folder's full path:" << endl;

		string imagePath;
		getline(cin, imagePath);

		if (imagePath == "q" || imagePath == "Q")
			break;

		vector<string> vecFiles;
		GetFiles(imagePath, vecFiles);
		
		MultiThreadDecodeFileFolder(vecFiles);
		
		cout << "Finish." << endl;
	}

	return 0;
}

