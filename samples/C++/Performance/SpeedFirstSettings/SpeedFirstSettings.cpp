#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <iostream>
#include "../../../../include/DynamsoftBarcodeReader.h"
#include "../../../../include/DynamsoftCommon.h"
using namespace std;
using namespace dynamsoft::dbr;

// Link libraries of DynamsoftBarcodeReader SDK for Windows.
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <conio.h>
#ifdef _WIN64
#pragma comment(lib, "../../../../lib/Windows/x64/DBRx64.lib")
#else
#pragma comment(lib, "../../../../lib/Windows/x86/DBRx86.lib")
#endif
#else
#include <sys/time.h>
#endif


unsigned long GetTiming()
{
#if defined(_WIN64) || defined(_WIN32)
	return GetTickCount();
#else
	struct timeval timing;
	gettimeofday(&timing, NULL);
	return timing.tv_sec * 1000 + timing.tv_usec / 1000;
#endif
}

class SpeedFirstSettings {
public:
	void configSpeedFirst(CBarcodeReader *reader) {
		PublicRuntimeSettings settings;
		reader->GetRuntimeSettings(&settings);
		settings.barcodeFormatIds = BF_EAN_13;
		settings.expectedBarcodesCount = 1;
		settings.scaleDownThreshold = 1200;

		settings.binarizationModes[0] = BM_LOCAL_BLOCK;
		settings.binarizationModes[1] = BM_SKIP;
		settings.binarizationModes[2] = BM_SKIP;
		settings.binarizationModes[3] = BM_SKIP;
		settings.binarizationModes[4] = BM_SKIP;
		settings.binarizationModes[5] = BM_SKIP;
		settings.binarizationModes[6] = BM_SKIP;
		settings.binarizationModes[7] = BM_SKIP;

		settings.localizationModes[0] = LM_SCAN_DIRECTLY;
		settings.localizationModes[1] = LM_SKIP;
		settings.localizationModes[2] = LM_SKIP;
		settings.localizationModes[3] = LM_SKIP;
		settings.localizationModes[4] = LM_SKIP;
		settings.localizationModes[5] = LM_SKIP;
		settings.localizationModes[6] = LM_SKIP;
		settings.localizationModes[7] = LM_SKIP;


		settings.deblurModes[0] = DM_BASED_ON_LOC_BIN;
		settings.deblurModes[1] = DM_THRESHOLD_BINARIZATION;
		settings.deblurModes[2] = DM_SKIP;
		settings.deblurModes[3] = DM_SKIP;
		settings.deblurModes[4] = DM_SKIP;
		settings.deblurModes[5] = DM_SKIP;
		settings.deblurModes[6] = DM_SKIP;
		settings.deblurModes[7] = DM_SKIP;
		settings.deblurModes[8] = DM_SKIP;
		settings.deblurModes[9] = DM_SKIP;

		settings.timeout = 100;

		reader->UpdateRuntimeSettings(&settings);
	

		reader->SetModeArgument("BinarizationModes", 0, "EnableFillBinaryVacancy", "0");

		reader->SetModeArgument("LocalizationModes", 0, "ScanDirection", "0");

	}
	void configSpeedFirstByTemplate(CBarcodeReader *reader) {
		int ret = reader->InitRuntimeSettingsWithFile("SpeedFirstTemplate.json", CM_OVERWRITE);
		if (ret != DBR_OK) {
			cout << "Error" << endl;
			exit(0);
		}
	}

	void outputResults(TextResultArray* barcodeResults, CBarcodeReader& reader, long costTime) {
		cout << "Cost time:" << costTime << "ms" << endl;
		if (barcodeResults != NULL && barcodeResults->resultsCount > 0)
		{
			// Process each result in a loop
			for (int i = 0; i < barcodeResults->resultsCount; ++i)
			{
				cout << "Result " << (i + 1) << ":" << endl;

				// 5.1. Get format of each barcode
				if (barcodeResults->results[i]->barcodeFormat != BF_NULL)
					cout << "    Barcode Format: " << barcodeResults->results[i]->barcodeFormatString << endl;
				else
					cout << "    Barcode Format: " << barcodeResults->results[i]->barcodeFormatString_2 << endl;

				// 5.2. Get text result of each barcode
				cout << "    Barcode Text: " << barcodeResults->results[i]->barcodeText << endl;
			}
		}
		else
		{
			cout << "No barcode detected." << endl;
		}

		// 6. Free the memory allocated for text results
		if (barcodeResults != NULL)
			CBarcodeReader::FreeTextResults(&barcodeResults);

	}
};

int main() {

	int errorCode = 0;
	char szErrorMsg[256];
	TextResultArray* barcodeResults = NULL;

	DM_DLSConnectionParameters paramters;
	CBarcodeReader::InitDLSConnectionParameters(&paramters);
	paramters.organizationID = const_cast<char*>("200001"); // replace it with your organization ID
	errorCode = CBarcodeReader::InitLicenseFromDLS(&paramters, szErrorMsg, 256);
	if (errorCode != DBR_OK)
	{
		cout << szErrorMsg << endl;
	}

	CBarcodeReader reader;
	SpeedFirstSettings sf;

	string fileName = "../../../../images/AllSupportedBarcodeTypes.png";
	unsigned long ullTimeBeg;
	unsigned long ullTimeEnd;

	cout << "Decode through PublicRuntimeSettings:" << endl;
	{
		sf.configSpeedFirst(&reader);
		ullTimeBeg = GetTiming();
		reader.DecodeFile(fileName.c_str(), "");
		ullTimeEnd = GetTiming();
		reader.GetAllTextResults(&barcodeResults);
		sf.outputResults(barcodeResults, reader, ullTimeEnd - ullTimeBeg);

	}
	cout << endl;
	cout << "Decode through parameters template:" << endl;
	{
		sf.configSpeedFirstByTemplate(&reader);
		ullTimeBeg = GetTiming();
		reader.DecodeFile(fileName.c_str(), "");
		ullTimeEnd = GetTiming();
		reader.GetAllTextResults(&barcodeResults);
		sf.outputResults(barcodeResults, reader, ullTimeEnd - ullTimeBeg);
	}
	return 0;
}

