#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#if defined(_WIN64) || defined(_WIN32)
#include <io.h>
#include <direct.h>
#include <time.h>
#define sleep _sleep
#else
#include <string.h>
#include <sys/io.h>
#include <dirent.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#endif
#if defined(_WIN64) || defined(_WIN32)
#define strcasecmp stricmp
#define separator "\\"
#else
#define separator "/"
#endif

#include "../../../../include/DynamsoftBarcodeReader.h"
#include "../../../../include/DynamsoftCommon.h"
using namespace std;
using namespace dynamsoft::dbr;

#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../../lib/Windows/x64/DBRx64.lib")
#else
#pragma comment(lib, "../../../../lib/Windows/x86/DBRx86.lib")
#endif
#endif

string folder = "images";
string tpDecoding		  = "templates/Decoding.json";
string tpDecodingBinImage = "templates/DecodingBinImage.json";

string tpHead = "{\"Version\":\"3.0\", \"ImageParameterContentArray\" : [{ \"Name\": \"Test\", \"MaxAlgorithmthreadCount\" : 4, \"ExpectedBarcodesCount\" : 0, \"ScaleDownThreshold\" : 700,";
string intermediateResultTypes[] = {
	"\"IntermediateResultTypes\" : [\"IRT_PREPROCESSED_IMAGE\"],",
	"\"IntermediateResultTypes\" : [\"IRT_BINARIZED_IMAGE\"],",
	"\"IntermediateResultTypes\" : [\"IRT_TYPED_BARCODE_ZONE\"],"
};

int gtmModesCount = 1;
string gtmModes[] = {
	//"\"GrayscaleTransformationModes\": [{\"Mode\": \"GTM_ORIGINAL\"}],",
	"\"GrayscaleTransformationModes\": [{\"Mode\": \"GTM_INVERTED\"}],"
};

int ipModesCount = 3;
string ipModes[] = {
	//"\"ImagePreprocessingModes\": [{\"Mode\": \"IPM_SHARPEN_SMOOTH\"}],",
	"\"ImagePreprocessingModes\": [{\"Mode\": \"IPM_SHARPEN_SMOOTH\",\"SharpenBlockSizeX\" : 5,\"SharpenBlockSizeY\" : 5,\"SmoothBlockSizeX\" : 5,\"SmoothBlockSizeY\" : 5}],",
	"\"ImagePreprocessingModes\": [{\"Mode\": \"IPM_GRAY_EQUALIZE\", \"Sensitivity\":9}],",
	"\"ImagePreprocessingModes\": [{\"Mode\": \"IPM_SKIP\"}],"
};

int binModesCount = 3;
string binModes[] = {
	"\"BinarizationModes\": [{\"Mode\": \"BM_LOCAL_BLOCK\", \"BlockSizeX\" : 30, \"BlockSizeY\" : 30, \"EnableFillBinaryVacancy\" : 0, \"ThreshValueCoefficient\" : 1}],",
	"\"BinarizationModes\": [{\"Mode\": \"BM_LOCAL_BLOCK\", \"BlockSizeX\" : 15, \"BlockSizeY\" : 15, \"EnableFillBinaryVacancy\" : 0, \"ThreshValueCoefficient\" : 0}],",
	"\"BinarizationModes\": [{\"Mode\": \"BM_LOCAL_BLOCK\", \"BlockSizeX\" : 8, \"BlockSizeY\" : 8, \"EnableFillBinaryVacancy\" : 0, \"ThreshValueCoefficient\" : 1}],"
};

int locModesCount = 2;
string locModes[] = {
	"\"LocalizationModes\": [{\"Mode\": \"LM_CONNECTED_BLOCKS\"}],",
	"\"LocalizationModes\": [{\"Mode\": \"LM_SCAN_DIRECTLY\"}],"
};
string terminatePhases[] = {
	"\"TerminatePhase\" : \"TP_IMAGE_PREPROCESSED\",",
	"\"TerminatePhase\" : \"TP_IMAGE_BINARIZED\",",
	"\"TerminatePhase\" : \"TP_BARCODE_TYPE_DETERMINED\","
};
string tpFoot = "\"TextureDetectionModes\": [{ \"Mode\": \"TDM_SKIP\" }], \"TimeOut\" : 100000 }]}";


char dbrKey[] = "DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9";

struct PathInfo
{
	char* name;
	bool isDir;
	const string strDir;
#if defined(_WIN64) || defined(_WIN32)
	_finddata_t findData;
	intptr_t handle;
#else
	struct dirent* pDirent;
	struct stat s_buf;
	DIR* pDir;
#endif
	PathInfo(const string& sDir) :strDir(sDir)
	{}
};
static bool ReadNext(PathInfo& pathInfo)
{
#if defined(_WIN64) || defined(_WIN32)
	if (_findnext(pathInfo.handle, &pathInfo.findData) == 0)
	{
		pathInfo.name = pathInfo.findData.name;
		pathInfo.isDir = ((pathInfo.findData.attrib & _A_SUBDIR) != 0);
		return true;
	}
#else
	pathInfo.pDirent = readdir(pathInfo.pDir);
	if (pathInfo.pDirent != NULL)
	{
		pathInfo.name = pathInfo.pDirent->d_name;
		string strTmpDir = pathInfo.strDir + "/" + pathInfo.name;
		stat(strTmpDir.c_str(), &pathInfo.s_buf);
		pathInfo.isDir = S_ISDIR(pathInfo.s_buf.st_mode);
		return true;
	}
#endif
	return false;
}
static bool OpenDirectory(PathInfo& pathInfo)
{
#if defined(_WIN64) || defined(_WIN32)
	string strTmpDir = pathInfo.strDir + "\\*.*";
	pathInfo.handle = _findfirst(strTmpDir.c_str(), &pathInfo.findData);
	if (pathInfo.handle == -1)
		return false;
	pathInfo.name = pathInfo.findData.name;
	pathInfo.isDir = ((pathInfo.findData.attrib & _A_SUBDIR) != 0);
#else
	stat(pathInfo.strDir.c_str(), &pathInfo.s_buf);
	pathInfo.pDir = opendir(pathInfo.strDir.c_str());
	if (!pathInfo.pDir)
		return false;
	if (!ReadNext(pathInfo))
		return false;
#endif
	return true;
}
static int CloseDirectory(PathInfo& pathInfo)
{
#if defined(_WIN64) || defined(_WIN32)
	return _findclose(pathInfo.handle);
#else
	return closedir(pathInfo.pDir);
#endif
}

inline unsigned long GetTiming()
{
#if defined(_WIN64) || defined(_WIN32)
	return clock();
#else
	struct timeval timing;
	gettimeofday(&timing, NULL);
	return timing.tv_sec * 1000  + timing.tv_usec / 1000;
#endif
}

int main(int argc, const char* argv[])
{
	char szErrorMsg[256];
	int iRet = CBarcodeReader::InitLicense(dbrKey, szErrorMsg, 256);
	if (iRet != 0)
	{
		cout << szErrorMsg << endl;
	}

	CBarcodeReader reader1, reader21, reader22, reader23, reader3;
	printf("*************************************************\r\n");
	printf("Welcome to Dynamsoft Barcode Reader Demo\r\n");
	printf("DBR version: %s\r\n", reader1.GetVersion());
	printf("*************************************************\r\n\r\n");

	PathInfo pathInfo(folder);
	if (!OpenDirectory(pathInfo))
	{
		printf("The directory %s does not exist.\r\n", folder.c_str());
		return -1;
	}
		
	unsigned long ullTimeBegin = 0;
	unsigned long ullTimeEnd = 0;
	unsigned long totalTime = 0;
	int totalImageCount = 0;
	int sucCount_Step1 = 0;
	int sucCount = 0;
	int totalBarcodeCount = 0;
	float totalTime_Step1 = 0;
	float totalTime_Step2 = 0;
	vector<string> failedImgs;
	do
	{
		if (pathInfo.isDir)
		{
			continue;
		}
		else
		{
			string fileName = pathInfo.name;
			size_t pos = fileName.find_last_of('.');
			if (pos != string::npos)
			{
				fileName = fileName.substr(pos);
			}
			if (strcasecmp(fileName.c_str(), ".bmp") != 0
				&& strcasecmp(fileName.c_str(), ".jpg") != 0
				&& strcasecmp(fileName.c_str(), ".jpeg") != 0
				&& strcasecmp(fileName.c_str(), ".pdf") != 0
				&& strcasecmp(fileName.c_str(), ".tif") != 0
				&& strcasecmp(fileName.c_str(), ".png") != 0
				&& strcasecmp(fileName.c_str(), ".tiff") != 0
				&& strcasecmp(fileName.c_str(), ".gif") != 0)
				continue;
			string strCurrentFilePath = folder + separator + pathInfo.name;
			const char* filePath = strCurrentFilePath.c_str();
			bool success = false;

			totalImageCount++;
			printf("%d: Image: %s\r\n", totalImageCount, filePath);

			//============== Step 1: check if can be decoded with general settings ===============

			// As an example, we set parameters based on the sample image in the template file defined by tpDecoding
			iRet = reader1.InitRuntimeSettingsWithFile(tpDecoding.c_str(), CM_OVERWRITE, szErrorMsg, 256);
			if (iRet != 0)
			{
				printf("Load %s error: %s\r\n", tpDecoding.c_str(), szErrorMsg);
				return -1;
			}

			ullTimeBegin = GetTiming();
			iRet = reader1.DecodeFile(filePath, "");
			ullTimeEnd = GetTiming();

			unsigned long curImgTime = ullTimeEnd - ullTimeBegin;
			totalTime_Step1 += curImgTime;

			TextResultArray* paryResult = NULL;
			reader1.GetAllTextResults(&paryResult);
			if (paryResult->resultsCount > 0)
			{
				printf("Result count: %d\r\n", paryResult->resultsCount);
				printf("Current image total time: %d ms\r\n", int(curImgTime));
				for (int i = 0; i < paryResult->resultsCount; ++i)
				{
					printf("%3d Format: %-18s Text: %s\r\n", i, paryResult->results[i]->barcodeFormatString, paryResult->results[i]->barcodeText);
				}
				printf("\r\n");

				sucCount_Step1++;
				totalBarcodeCount += paryResult->resultsCount;
				success = true;
			}
			else {
				//============== Step 2: generate and decode intermediate result ===============

				// GrayscaleTransformationModes loop
				for (int gtmi = 0; gtmi < gtmModesCount; gtmi++)
				{
					if (success) break;
					string settingsT = tpHead + gtmModes[gtmi];

					// ImagePreProcessingModes loop
					for (int ipmi = 0; ipmi < ipModesCount; ipmi++)
					{
						if (success) break;
						string settingsT2 = settingsT + ipModes[ipmi];
						string settingsGetGrayImg = settingsT2 + terminatePhases[0] + intermediateResultTypes[0] + tpFoot;

						iRet = reader21.InitRuntimeSettingsWithString(settingsGetGrayImg.c_str(), CM_OVERWRITE, szErrorMsg, 256);
						if (iRet != 0)
						{
							printf("ImagePreprocessingMode %d error: %s\r\n", ipmi, szErrorMsg);
							continue;
						}

						ullTimeBegin = GetTiming();
						iRet = reader21.DecodeFile(filePath, "");
						ullTimeEnd = GetTiming();
						curImgTime += ullTimeEnd - ullTimeBegin;
						totalTime_Step2 += ullTimeEnd - ullTimeBegin;

						// binarizationModes loop
						IntermediateResultArray* pIMRA21 = NULL;
						reader21.GetIntermediateResults(&pIMRA21);
						if (pIMRA21 != NULL)
						{
							for (int bini = 0; bini < binModesCount; bini++)
							{
								if (success) break;
								string settingsT3 = settingsT2 + binModes[bini];
								string settingsGetBinImg = settingsT3 + terminatePhases[1] + intermediateResultTypes[1] + tpFoot;

								iRet = reader22.InitRuntimeSettingsWithString(settingsGetBinImg.c_str(), CM_OVERWRITE, szErrorMsg, 256);
								if (iRet != 0)
								{
									printf("BinarizationMode %d error: %s\r\n", bini, szErrorMsg);
									continue;
								}

								ullTimeBegin = GetTiming();
								iRet = reader22.DecodeIntermediateResults(pIMRA21, "");
								ullTimeEnd = GetTiming();
								curImgTime += ullTimeEnd - ullTimeBegin;
								totalTime_Step2 += ullTimeEnd - ullTimeBegin;

								// localizationModes loop
								IntermediateResultArray* pIMRA22 = NULL;
								reader22.GetIntermediateResults(&pIMRA22);
								if (pIMRA22 != NULL)
								{
									string noIpmSettings = settingsT + ipModes[ipModesCount-1] + binModes[bini];
									for (int loci = 0; loci < locModesCount; loci++)
									{
										if (success) break;
										string settingsT4 = noIpmSettings + locModes[loci];
										string settingsGetZone = settingsT4 + terminatePhases[2] + intermediateResultTypes[2] + tpFoot;

										iRet = reader23.InitRuntimeSettingsWithString(settingsGetZone.c_str(), CM_OVERWRITE, szErrorMsg, 256);
										if (iRet != 0)
										{
											printf("LocalizationMode %d error: %s\r\n", loci, szErrorMsg);
											continue;
										}

										ullTimeBegin = GetTiming();
										iRet = reader23.DecodeIntermediateResults(pIMRA22, "");
										ullTimeEnd = GetTiming();
										curImgTime += ullTimeEnd - ullTimeBegin;
										totalTime_Step2 += ullTimeEnd - ullTimeBegin;

										// decode intermediate results
										IntermediateResultArray* pIMRA23 = NULL;
										reader23.GetIntermediateResults(&pIMRA23);
										if (pIMRA23 != NULL)
										{
											PIntermediateResult& pIMR = pIMRA22->results[0];
											const ImageData* img = (const ImageData*)pIMR->results[0];

											// As an example, we use default setting here.
											// You can also set some parameters via the template file defined by tpDecodingBinImage
											//iRet = reader3.InitRuntimeSettingsWithFile(tpDecodingBinImage.c_str(), CM_OVERWRITE, szErrorMsg, 256);
											//if (iRet != 0)
											//{
											//	printf("Load %s error: %s\r\n", tpDecodingBinImage.c_str(), szErrorMsg);
											//	return -1;
											//}

											ullTimeBegin = GetTiming();
											reader3.DecodeBuffer(img->bytes, img->width, img->height, img->stride, img->format, "");
											ullTimeEnd = GetTiming();
											curImgTime += ullTimeEnd - ullTimeBegin;
											totalTime_Step2 += ullTimeEnd - ullTimeBegin;

											TextResultArray* paryResult = NULL;
											reader3.GetAllTextResults(&paryResult);
											if (paryResult->resultsCount > 0)
											{
												printf("Result count: %d\r\n", paryResult->resultsCount);
												printf("Current image total time: %d ms\r\n", int(curImgTime));
												for (int i = 0; i < paryResult->resultsCount; ++i)
												{
													printf("%3d Format: %-18s Text: %s\r\n", i, paryResult->results[i]->barcodeFormatString, paryResult->results[i]->barcodeText);
												}
												printf("\r\n");
												printf("%s\r\n%s\r\n%s\r\n%s\r\n", gtmModes[gtmi].c_str(), ipModes[ipmi].c_str(), binModes[bini].c_str(), locModes[loci].c_str());
												success = true;
												sucCount++;
												totalBarcodeCount += paryResult->resultsCount;
											}
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			if (!success)
			{
				printf("Result count: %d\r\n", 0);
				printf("Current image total time: %d ms\r\n", int(curImgTime));
				failedImgs.push_back(strCurrentFilePath);
			}
			totalTime += curImgTime;
		}

	} while (ReadNext(pathInfo));
	
	if (failedImgs.size() > 0)
		printf("failedImgs:\r\n");
	for (int i = 0; i < failedImgs.size(); ++i)
	{
		printf("%d: %s\r\n", i, failedImgs[i].c_str());
	}
	if (failedImgs.size() > 0)
		printf("\r\n");

	if (totalImageCount > 0)
	{
		std::cout << "Total Image Count: " << totalImageCount << endl;
		std::cout << "Step1 Success Image Count: " << sucCount_Step1 << endl;
		std::cout << "Step2 Success Image Count: " << sucCount << endl;
		std::cout << "Total Barcode Count: " << totalBarcodeCount << endl;

		std::cout << "Total Time: " << totalTime << " ms" << endl;
		std::cout << "Avg Time: " << (totalTime) * 1.f / totalImageCount << " ms" << endl;
		std::cout << "step 1 Avg Time: " << totalTime_Step1 / totalImageCount << " ms" << endl;
		std::cout << "step 2 Avg Time: " << totalTime_Step2 / totalImageCount << " ms" << endl;
	}

	std::cin.ignore();
	return 0;
}