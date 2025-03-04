#if defined(_WIN64) || defined(_WIN32)
#include <io.h>
#else
#ifdef DM_ARM
#include <sys/uio.h>
#else
#include <sys/io.h>
#endif
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include <time.h>
#include <iostream>
#include "DbrBarcodeFileReader.h"

CDbrBarcodeFileReader::CDbrBarcodeFileReader()
{
	m_pCVR = new CCaptureVisionRouter;
}

CDbrBarcodeFileReader::~CDbrBarcodeFileReader()
{
	if (m_pCVR)
	{
		delete m_pCVR, m_pCVR = NULL;
	}
}

void CDbrBarcodeFileReader::Run()
{
	if (!m_pCVR)
	{
		cout << "Get instance failed." << endl;
		return;
	}
	LoadRuntimeSettings("templates");
	if (m_listSettingsFile.size() > 0)
	{
		RunWithRuntimeSettings();
	}
	else
	{
		m_currentOutputFileName = CreateOutputFileName("DEFAULT"); 
		CBarcodeFileReader::Run();
	}
	m_listSettingsFile.clear();
}


void CDbrBarcodeFileReader::LoadRuntimeSettings(string strSettingFilePath)
{
	PathInfo pathInfo(strSettingFilePath);
	string strTmpDir;
	vector<string> listDir;
	if (!OpenDirectory(pathInfo))
	{		
		return;
	}
	do
	{
		if (pathInfo.isDir)
		{
			if (strcmp(pathInfo.name, ".") == 0 || strcmp(pathInfo.name, "..") == 0)
				continue;
			strTmpDir = strSettingFilePath + separator + pathInfo.name;
			listDir.push_back(strTmpDir);
		}
		else
		{
			string fileName = pathInfo.name;
			size_t pos = fileName.find_last_of('.'); // find index of the last '.'
			if (pos != string::npos) // if pos exits
			{
				fileName = fileName.substr(pos);
			}
			if (strcasecmp(fileName.c_str(), ".json") != 0)
				continue;			
			string strCurrentFilePath = strSettingFilePath + separator + pathInfo.name;
			m_listSettingsFile.push_back(strCurrentFilePath);
		}

	} while (ReadNext(pathInfo));

	for (size_t i = 0; i < listDir.size(); i++)
	{
		LoadRuntimeSettings(listDir.at(i));
	}
	CloseDirectory(pathInfo);
}

void CDbrBarcodeFileReader::RunWithRuntimeSettings()
{
	CBarcodeStatisticsRecorder::RUNNING_TRACE_INFO traceInfo = m_pBarcodeStatisticsRecorder->LoadReaderRunningTrace();	
	bool bFindLastSetting = false;
	for (int i = 0; i < m_listSettingsFile.size(); i++)
	{
		traceInfo = m_pBarcodeStatisticsRecorder->LoadReaderRunningTrace();
		if (bFindLastSetting || traceInfo.DECODE_SETTING_FILE.empty()) {
			traceInfo.DECODE_SETTING_FILE = m_listSettingsFile.at(i);
			m_pBarcodeStatisticsRecorder->RecordReaderRunningTrace(traceInfo);
			if (!bFindLastSetting)
				bFindLastSetting = true;
		}
		else 
		{
#if defined(_WIN64) || defined(_WIN32)
			if (stricmp(traceInfo.DECODE_SETTING_FILE.c_str(), m_listSettingsFile.at(i).c_str()) != 0)
#else
			if (strcmp(traceInfo.DECODE_SETTING_FILE.c_str(), m_listSettingsFile.at(i).c_str()) != 0)
#endif
			continue;
			else
				bFindLastSetting = true;
		}

		string filePath = m_listSettingsFile.at(i);
		char szErrorMsgBuffer[1024] = { 0 };
		int nErrorCode = -1;
		nErrorCode = m_pCVR->InitSettingsFromFile(filePath.c_str(), szErrorMsgBuffer, sizeof(szErrorMsgBuffer));
		if (nErrorCode != 0)
		{
			cout <<"Init runtime settings file("+ filePath+") failed:"<<string(szErrorMsgBuffer) << endl;
		}
		else {
			int pos1 = filePath.rfind(separator);
			int pos2 = filePath.rfind('.');
			string strFileName = filePath.substr(pos1+1, pos2 - pos1-1);
			m_currentOutputFileName = CreateOutputFileName(strFileName);
			CBarcodeFileReader::Run();
		}
	}	
}

bool CDbrBarcodeFileReader::ReadFileBarcodes(const string strFilePath, CBarcodeStatisticsRecorder::DecodeResultInfo& decodeResultInfo)
{
	bool bret = true;
	clock_t start, end;

	///////////////////////////////////////////////////
	start = clock();
	int nErrorCode = -1;
	CCapturedResult* result = m_pCVR->Capture(strFilePath.c_str());
	end = clock();
	decodeResultInfo.dDecodeTime = ((double)(end - start) / CLOCKS_PER_SEC * 1000);

	if (result->GetErrorCode() != ErrorCode::EC_OK)
	{
		decodeResultInfo.strErrorMessage = result->GetErrorString();
		if (result->GetErrorCode() != ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
			bret = false;
	}

	CDecodedBarcodesResult* barcodeResult = result->GetDecodedBarcodesResult();
	for (int i = 0; barcodeResult != NULL && i < barcodeResult->GetItemsCount(); i++)
	{
		CBarcodeStatisticsRecorder::BCODE_VALUE bcodeValue;
		const CBarcodeResultItem* barcodeResultItem = barcodeResult->GetItem(i);
		bcodeValue.strTextMessage = barcodeResultItem->GetText();
		bcodeValue.strCodeFormat = barcodeResultItem->GetFormatString();
		unsigned char* tempByte = barcodeResultItem->GetBytes();
		int byteLength = barcodeResultItem->GetBytesLength();
		bcodeValue.strHexMessage = ToHexString(tempByte, byteLength);
		decodeResultInfo.listCodes.push_back(bcodeValue);
	}
	if (barcodeResult)
		barcodeResult->Release();
	if (result)
	{
		result->Release();
	}

	return bret;
}

string CDbrBarcodeFileReader::CreateOutputFileName(string defultName)
{
	string strFileName = defultName;
	string currentOutputFileName = "DBR";
	currentOutputFileName += "_T_" + strFileName + "_" + GetCurrentTimeString();
	return currentOutputFileName;
}

	
