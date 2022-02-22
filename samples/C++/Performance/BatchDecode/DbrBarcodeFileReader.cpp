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
	// Initialize license
	/*
	// By setting organization ID as "200001", a free public trial license will be used for license verification.
	// Note that network connection is required for this license to work.
	//
	// When using your own license, locate the following line and specify your Organization ID.
	// organizationID = "200001";
	//
	// If you don't have a license yet, you can request a trial from https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
	*/
	int errorCode = 0;
	char szErrorMsg[256];
	DM_DLSConnectionParameters paramters;
	dynamsoft::dbr::CBarcodeReader::InitDLSConnectionParameters(&paramters);
	paramters.organizationID = const_cast<char*>("200001"); // replace it with your organization ID
	errorCode = dynamsoft::dbr::CBarcodeReader::InitLicenseFromDLS(&paramters, szErrorMsg, 256);
	if (errorCode != DBR_OK)
	{
		cout << szErrorMsg << endl;
	}

	m_pBarcodeReader = new dynamsoft::dbr::CBarcodeReader();
}

CDbrBarcodeFileReader::~CDbrBarcodeFileReader()
{

	SAFE_DELETE(m_pBarcodeReader);
}

void CDbrBarcodeFileReader::Run()
{
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
		nErrorCode = m_pBarcodeReader->InitRuntimeSettingsWithFile(filePath.c_str(), ConflictMode::CM_OVERWRITE, szErrorMsgBuffer, sizeof(szErrorMsgBuffer));
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
	nErrorCode = m_pBarcodeReader->DecodeFile(strFilePath.c_str());
	end = clock();
	decodeResultInfo.dDecodeTime = ((double)(end - start) / CLOCKS_PER_SEC * 1000);
	TextResultArray* pTextResultArray = NULL;

	nErrorCode = m_pBarcodeReader->GetAllTextResults(&pTextResultArray);
	if (nErrorCode != 0)
	{
		decodeResultInfo.strErrorMessage = m_pBarcodeReader->GetErrorString(nErrorCode);
		bret = false;
	}


	for (int i = 0; pTextResultArray != NULL && i < (pTextResultArray)->resultsCount; i++)
	{
		CBarcodeStatisticsRecorder::BCODE_VALUE bcodeValue;
		bcodeValue.strTextMessage = (pTextResultArray)->results[i]->barcodeText;
		if ((pTextResultArray)->results[i]->barcodeFormat != 0)
		{
			bcodeValue.strCodeFormat = (pTextResultArray)->results[i]->barcodeFormatString;
		}
		else
		{
			bcodeValue.strCodeFormat = (pTextResultArray)->results[i]->barcodeFormatString_2;
		}
		unsigned char* tempByte = (pTextResultArray)->results[i]->barcodeBytes;
		int byteLength = (pTextResultArray)->results[i]->barcodeBytesLength;
		bcodeValue.strHexMessage = ToHexString(tempByte, byteLength);
		decodeResultInfo.listCodes.push_back(bcodeValue);
	}
	if (pTextResultArray != NULL)
	{
		m_pBarcodeReader->FreeTextResults(&pTextResultArray);
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

	
