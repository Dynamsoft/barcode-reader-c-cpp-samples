#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include "../../../../include/DynamsoftBarcodeReader.h"
#include "../../../../include/DynamsoftCommon.h"
using namespace std;
using namespace dynamsoft::dbr;

// Link libraries of DynamsoftBarcodeReader SDK for Windows.
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <conio.h>
#include <iostream>
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
		// Obtain current runtime settings of instance.
		PublicRuntimeSettings settings;
		reader->GetRuntimeSettings(&settings);

		// Parameter 1. Set expected barcode formats
		// The simpler barcode format, the faster decoding speed.
		// Here we use OneD barcode format to demonstrate.
		settings.barcodeFormatIds = BF_EAN_13;

		// Parameter 2. Set expected barcode count
		// The less barcode count, the faster decoding speed.
		settings.expectedBarcodesCount = 1;

		// Parameter 3. Set the threshold for the image shrinking for localization.
		// The smaller the threshold, the smaller the image shrinks.  The default value is 2300.
		settings.scaleDownThreshold = 1200;

		// Parameter 4. Set the binarization mode for convert grayscale image to binary image.
		// Mostly, the less binarization modes set, the faster decoding speed.
		settings.binarizationModes[0] = BM_LOCAL_BLOCK;

		// Parameter 5. Set localization mode.
		// LM_SCAN_DIRECTLY: Localizes barcodes quickly. It is both for OneD and TweD barcodes. This mode is recommended in interactive scenario.
		settings.localizationModes[0] = LM_SCAN_DIRECTLY;
		settings.localizationModes[1] = LM_SKIP;
		settings.localizationModes[2] = LM_SKIP;
		settings.localizationModes[3] = LM_SKIP;

		// LM_ONED_FAST_SCAN: Localizing barcodes quickly. However, it is only for OneD barcodes. It is also recommended in interactive scenario.

		// Parameter 6. Reduce deblurModes setting
		// DeblurModes will improve the readability and accuracy but decrease the reading speed.
		// Please update your settings here if you want to enable different Deblurmodes.
		settings.deblurModes[0] = DM_BASED_ON_LOC_BIN;
		settings.deblurModes[1] = DM_THRESHOLD_BINARIZATION;

		// Parameter 7. Set timeout(ms) if the application is very time sensitive.
		// If timeout, the decoding thread will exit at the next check point.
		settings.timeout = 100;

		reader->UpdateRuntimeSettings(&settings);
	
		// Other potentially accelerated arguments of various modes.

		// Argument 4.a Disable the EnableFillBinaryVacancy argument.
		// Local block binarization process might cause vacant area in barcode. The barcode reader will fill the vacant black by default (default value 1). Set the value 0 to disable this process.
		reader->SetModeArgument("BinarizationModes", 0, "EnableFillBinaryVacancy", "0");

		// Argument 5.a Sets the scan direction when searching barcode.
		// It is valid only when the type of LocalizationMode is LM_ONED_FAST_SCAN or LM_SCAN_DIRECTLY.
		// 0: Both vertical and horizontal direction.
		// 1: Vertical direction.
		// 2: Horizontal direction.
		// Read more about localization mode members: https://www.dynamsoft.com/barcode-reader/parameters/enum/parameter-mode-enums.html?ver=latest#localizationmode
		reader->SetModeArgument("LocalizationModes", 0, "ScanDirection", "0");

	}
	void configSpeedFirstByTemplate(CBarcodeReader *reader) {
		// Compared with PublicRuntimeSettings, parameter templates have a richer ability to control parameter details.
		// Please refer to the parameter explanation in "SpeedFirstTemplate.json" to understand how to control speed first.
		int ret = reader->InitRuntimeSettingsWithFile("SpeedFirstTemplate.json", CM_OVERWRITE);
		if (ret != DBR_OK) {
			cout << "Error" << endl;
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

				//Get format of each barcode
				if (barcodeResults->results[i]->barcodeFormat != BF_NULL)
					cout << "    Barcode Format: " << barcodeResults->results[i]->barcodeFormatString << endl;
				else
					cout << "    Barcode Format: " << barcodeResults->results[i]->barcodeFormatString_2 << endl;

				//Get text result of each barcode
				cout << "    Barcode Text: " << barcodeResults->results[i]->barcodeText << endl;
			}
		}
		else
		{
			cout << "No barcode detected." << endl;
		}

		//Free the memory allocated for text results
		if (barcodeResults != NULL)
			CBarcodeReader::FreeTextResults(&barcodeResults);

	}
};

int main() {

	int errorCode = 0;
	char szErrorMsg[256];
	TextResultArray* barcodeResults = NULL;

	// Initialize license
	/*
	// By setting organizaion ID as "200001", a 7-day trial license will be used for license verification.
	// Note that network connection is required for this license to work.
	//
	// When using your own license, locate the following line and specify your Organization ID.
	// organizationID = "200001";
	//
	// If you don't have a license yet, you can request a trial from https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
	*/
	DM_DLSConnectionParameters paramters;
	CBarcodeReader::InitDLSConnectionParameters(&paramters);
	paramters.organizationID = const_cast<char*>("200001"); // replace it with your organization ID
	errorCode = CBarcodeReader::InitLicenseFromDLS(&paramters, szErrorMsg, 256);
	if (errorCode != DBR_OK)
	{
		cout << szErrorMsg << endl;
	}

	//Create an instance of Barcode Reader
	CBarcodeReader reader;
	SpeedFirstSettings sf;

	string fileName = "../../../../images/AllSupportedBarcodeTypes.png";
	unsigned long ullTimeBeg;
	unsigned long ullTimeEnd;

	// There are two ways to configure runtime parameters. One is through PublicRuntimeSettings, the other is through parameters template.
	cout << "Decode through PublicRuntimeSettings:" << endl;
	{
		// config through PublicRuntimeSettings
		sf.configSpeedFirst(&reader);

		ullTimeBeg = GetTiming();
		
		// Decode barcodes from an image file by current runtime settings. The second parameter value "" means to decode through the current PublicRuntimeSettings.
		reader.DecodeFile(fileName.c_str(), "");

		ullTimeEnd = GetTiming();
		
		//Get all barcode results
		reader.GetAllTextResults(&barcodeResults);

		// Output the barcode format and barcode text.
		sf.outputResults(barcodeResults, reader, ullTimeEnd - ullTimeBeg);

	}
	cout << endl;
	cout << "Decode through parameters template:" << endl;
	{
		// config through parameters template
		sf.configSpeedFirstByTemplate(&reader);

		ullTimeBeg = GetTiming();
		
		// Decode barcodes from an image file by template.
		reader.DecodeFile(fileName.c_str(), "");
		
		ullTimeEnd = GetTiming();

		//Get all barcode results
		reader.GetAllTextResults(&barcodeResults);

		// Output the barcode format and barcode text.
		sf.outputResults(barcodeResults, reader, ullTimeEnd - ullTimeBeg);
	}
	cout << "Press any key to quit..." << endl;
    cin.ignore();
    return 0;
}

