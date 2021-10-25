#pragma once
#include "BarcodeFileReader.h"

// Include headers of DynamsoftBarcodeReader SDK
#include "../../../../include/DynamsoftBarcodeReader.h"
#include "../../../../include/DynamsoftCommon.h"

// Link libraries of DynamsoftBarcodeReader SDK for Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../../lib/Windows/x64/DBRx64.lib")
#else
#pragma comment(lib, "../../../../lib/Windows/x86/DBRx86.lib")
#endif
#endif

class CBarcodeReader;
class CDbrBarcodeFileReader :
	public CBarcodeFileReader
{
public:
	CDbrBarcodeFileReader();
	~CDbrBarcodeFileReader();
public:	
	void Run();
	void LoadRuntimeSettings(std::string strSettingFilePath);
protected:
	virtual bool ReadFileBarcodes(const std::string strFilePath, CBarcodeStatisticsRecorder::DecodeResultInfo &decodeResultInfo);
private:
	string CreateOutputFileName(string defultName);
	void RunWithRuntimeSettings();
private:
	
	vector<string>  m_listSettingsFile;
	dynamsoft::dbr::CBarcodeReader * m_pBarcodeReader;
};

