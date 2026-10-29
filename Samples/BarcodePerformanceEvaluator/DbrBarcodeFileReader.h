#pragma once
#include "BarcodeFileReader.h"

// Include headers of DynamsoftBarcodeReader SDK
#include "../../Include/DynamsoftCaptureVisionRouter.h"

using namespace std;
using namespace dynamsoft::license;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;

// Link libraries of DynamsoftBarcodeReader SDK for Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#endif
#endif

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
	CCaptureVisionRouter* cvRouter;
};

