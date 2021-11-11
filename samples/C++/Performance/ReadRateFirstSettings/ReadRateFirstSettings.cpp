#include <string>
#include <iostream>

#include "../../../../include/DynamsoftBarcodeReader.h"
#include "../../../../include/DynamsoftCommon.h"

using namespace std;
using namespace dynamsoft::dbr;

// Link libraries of DynamsoftBarcodeReader SDK for Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../../lib/Windows/x64/DBRx64.lib")
#else
#pragma comment(lib, "../../../../lib/Windows/x86/DBRx86.lib")
#endif
#endif

class ReadRateFirstSettings 
{
public:
	void configReadRateFirst(CBarcodeReader *reader) 
	{
		// Obtain current runtime settings of instance.
		PublicRuntimeSettings settings;
		reader->GetRuntimeSettings(&settings);

		// Parameter 1. Set expected barcode formats
		// Here the barcode scanner will try to find the maximal barcode formats.
		settings.barcodeFormatIds = BF_ALL;
		settings.barcodeFormatIds_2 = BF2_DOTCODE | BF2_POSTALCODE;

		// Parameter 2. Set expected barcode count.
	   // Here the barcode scanner will try to find 64 barcodes.
	   // If the result count does not reach the expected amount, the barcode scanner will try other algorithms 
		settings.expectedBarcodesCount = 64; 

		// Parameter 3. Set more binarization modes.
		settings.binarizationModes[0] = BM_LOCAL_BLOCK;
		settings.binarizationModes[1] = BM_THRESHOLD;

		// Parameter 4. Set more localization modes.
		// LocalizationModes are all enabled as default. Barcode reader will automatically switch between the modes and try decoding continuously until timeout or the expected barcode count is reached.
		// Please manually update the enabled modes list or change the expected barcode count to promote the barcode scanning speed.
		// Read more about localization mode members: https://www.dynamsoft.com/barcode-reader/parameters/enum/parameter-mode-enums.html?ver=latest#localizationmode
		settings.localizationModes[0] = LM_CONNECTED_BLOCKS;
		settings.localizationModes[1] = LM_SCAN_DIRECTLY;
		settings.localizationModes[2] = LM_STATISTICS;
		settings.localizationModes[3] = LM_LINES;
		settings.localizationModes[4] = LM_STATISTICS_MARKS;
		settings.localizationModes[5] = LM_STATISTICS_POSTAL_CODE;

		// Parameter 5. Set more deblur modes.
		// DeblurModes are all enabled as default. Barcode reader will automatically switch between the modes and try decoding continuously until timeout or the expected barcode count is reached.
		// Please manually update the enabled modes list or change the expected barcode count to promote the barcode scanning speed.
		//Read more about deblur mode members: https://www.dynamsoft.com/barcode-reader/parameters/enum/parameter-mode-enums.html#deblurmode
		settings.deblurModes[0] = DM_DIRECT_BINARIZATION;
		settings.deblurModes[1] = DM_THRESHOLD_BINARIZATION;
		settings.deblurModes[2] = DM_GRAY_EQUALIZATION;
		settings.deblurModes[3] = DM_SMOOTHING;
		settings.deblurModes[4] = DM_MORPHING;
		settings.deblurModes[5] = DM_DEEP_ANALYSIS;
		settings.deblurModes[6] = DM_SHARPENING;

		// Parameter 6. Set scale up modes.
		// It is a parameter to control the process for scaling up an image used for detecting barcodes with small module size
		settings.scaleUpModes[0] = SUM_AUTO;

		// Parameter 7. Set grayscale transformation modes.
		// By default, the library can only locate the dark barcodes that stand on a light background. "GTM_INVERTED":The image will be transformed into inverted grayscale.
		settings.furtherModes.grayscaleTransformationModes[0] = GTM_ORIGINAL;
		settings.furtherModes.grayscaleTransformationModes[1] = GTM_INVERTED;

		// Parameter 8. Enable dpm modes.
		// It is a parameter to control how to read direct part mark (DPM) barcodes.
		settings.furtherModes.dpmCodeReadingModes[0] = DPMCRM_GENERAL;

		// Parameter 9. Increase timeout(ms). The barcode scanner will have more chances to find the expected barcode until it times out
		settings.timeout = 30000;

		// Apply the new settings to the instance
		reader->UpdateRuntimeSettings(&settings);
	}
	void configReadFirstByTemplate(CBarcodeReader *reader) 
	{
		// Compared with PublicRuntimeSettings, parameter templates have a richer ability to control parameter details.
		// Please refer to the parameter explanation in "ReadRateFirstTemplate.json" to understand how to control read rate first.
		int ret = reader->InitRuntimeSettingsWithFile("ReadRateFirstTemplate.json", CM_OVERWRITE);
		if (ret != DBR_OK) 
		{
			cout << "Error" << endl;
		}
	}
	void outputResults(TextResultArray* barcodeResults, CBarcodeReader& reader)
	{
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
	// By setting organization ID as "200001", a 7-day trial license will be used for license verification.
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
	
	// Create an instance of Barcode Reader
	CBarcodeReader reader;
	ReadRateFirstSettings rf;

	string fileName = "../../../../images/AllSupportedBarcodeTypes.png";

	// There are two ways to configure runtime parameters. One is through PublicRuntimeSettings, the other is through parameters template.
	cout << "Decode through PublicRuntimeSettings:" << endl;
	{
		//	config through PublicRuntimeSettings
		rf.configReadRateFirst(&reader);
		
		//	Decode barcodes from an image file by current runtime settings. The second parameter value "" means to decode through the current PublicRuntimeSettings.
		errorCode=reader.DecodeFile(fileName.c_str(), "");
		
		//	Get all barcode results
		reader.GetAllTextResults(&barcodeResults);

		//	Output the barcode format and barcode text.
		rf.outputResults(barcodeResults, reader);
	}
	cout << endl;
	cout << "Decode through parameters template:" << endl;
	{
		// config through parameters template
		rf.configReadFirstByTemplate(&reader);

		// Decode barcodes from an image file by template.
		reader.DecodeFile(fileName.c_str(), "");

		//	Get all barcode results
		reader.GetAllTextResults(&barcodeResults);

		// Output the barcode format and barcode text.
		rf.outputResults(barcodeResults, reader);
	}
	cout << "Press any key to quit..." << endl;
    cin.ignore();
   	return 0;
}



