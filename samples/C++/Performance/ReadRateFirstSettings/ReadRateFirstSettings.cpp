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
		
		PublicRuntimeSettings settings;
		reader->GetRuntimeSettings(&settings);
		settings.barcodeFormatIds = BF_ALL;
		settings.barcodeFormatIds_2 = BF2_DOTCODE | BF2_POSTALCODE;
		settings.expectedBarcodesCount = 64; 
		settings.binarizationModes[0] = BM_LOCAL_BLOCK;
		settings.binarizationModes[1] = BM_THRESHOLD;
		settings.binarizationModes[2] = BM_SKIP;
		settings.binarizationModes[3] = BM_SKIP;
		settings.binarizationModes[4] = BM_SKIP;
		settings.binarizationModes[5] = BM_SKIP;
		settings.binarizationModes[6] = BM_SKIP;
		settings.binarizationModes[7] = BM_SKIP;

		settings.localizationModes[0] = LM_CONNECTED_BLOCKS;
		settings.localizationModes[1] = LM_SCAN_DIRECTLY;
		settings.localizationModes[2] = LM_STATISTICS;
		settings.localizationModes[3] = LM_LINES;
		settings.localizationModes[4] = LM_STATISTICS_MARKS;
		settings.localizationModes[5] = LM_STATISTICS_POSTAL_CODE;
		settings.localizationModes[6] = LM_SKIP;
		settings.localizationModes[7] = LM_SKIP;

		settings.deblurModes[0] = DM_DIRECT_BINARIZATION;
		settings.deblurModes[1] = DM_THRESHOLD_BINARIZATION;
		settings.deblurModes[2] = DM_GRAY_EQUALIZATION;
		settings.deblurModes[3] = DM_SMOOTHING;
		settings.deblurModes[4] = DM_MORPHING;
		settings.deblurModes[5] = DM_DEEP_ANALYSIS;
		settings.deblurModes[6] = DM_SHARPENING;
		settings.deblurModes[7] = DM_SKIP;
		settings.deblurModes[8] = DM_SKIP;
		settings.deblurModes[9] = DM_SKIP;

		settings.scaleUpModes[0] = SUM_AUTO;
		settings.scaleUpModes[1] = SUM_SKIP;
		settings.scaleUpModes[2] = SUM_SKIP;
		settings.scaleUpModes[3] = SUM_SKIP;
		settings.scaleUpModes[4] = SUM_SKIP;
		settings.scaleUpModes[5] = SUM_SKIP;
		settings.scaleUpModes[6] = SUM_SKIP;
		settings.scaleUpModes[7] = SUM_SKIP;

		settings.furtherModes.grayscaleTransformationModes[0] = GTM_ORIGINAL;
		settings.furtherModes.grayscaleTransformationModes[1] = GTM_INVERTED;
		settings.furtherModes.grayscaleTransformationModes[2] = GTM_SKIP;
		settings.furtherModes.grayscaleTransformationModes[3] = GTM_SKIP;
		settings.furtherModes.grayscaleTransformationModes[4] = GTM_SKIP;
		settings.furtherModes.grayscaleTransformationModes[5] = GTM_SKIP;
		settings.furtherModes.grayscaleTransformationModes[6] = GTM_SKIP;
		settings.furtherModes.grayscaleTransformationModes[7] = GTM_SKIP;

		settings.furtherModes.dpmCodeReadingModes[0] = DPMCRM_GENERAL;
		settings.furtherModes.dpmCodeReadingModes[1] = DPMCRM_SKIP;
		settings.furtherModes.dpmCodeReadingModes[2] = DPMCRM_SKIP;
		settings.furtherModes.dpmCodeReadingModes[3] = DPMCRM_SKIP;
		settings.furtherModes.dpmCodeReadingModes[4] = DPMCRM_SKIP;
		settings.furtherModes.dpmCodeReadingModes[5] = DPMCRM_SKIP;
		settings.furtherModes.dpmCodeReadingModes[6] = DPMCRM_SKIP;
		settings.furtherModes.dpmCodeReadingModes[7] = DPMCRM_SKIP;

		settings.timeout = 30000;

		reader->UpdateRuntimeSettings(&settings);
	}
	void configReadFirstByTemplate(CBarcodeReader *reader) 
	{
		int ret = reader->InitRuntimeSettingsWithFile("ReadRateFirstTemplate.json", CM_OVERWRITE);
		if (ret != DBR_OK) 
		{
			cout << "Error" << endl;
			exit(0);
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
	ReadRateFirstSettings rf;

	string fileName = "../../../../images/AllSupportedBarcodeTypes.png";

	cout << "Decode through PublicRuntimeSettings:" << endl;
	{
		rf.configReadRateFirst(&reader);
		errorCode=reader.DecodeFile(fileName.c_str(), "");
		reader.GetAllTextResults(&barcodeResults);
		rf.outputResults(barcodeResults, reader);
	}
	cout << endl;
	cout << "Decode through parameters template:" << endl;
	{
		rf.configReadFirstByTemplate(&reader);
		reader.DecodeFile(fileName.c_str(), "");
		reader.GetAllTextResults(&barcodeResults);
		rf.outputResults(barcodeResults, reader);
	}

}



