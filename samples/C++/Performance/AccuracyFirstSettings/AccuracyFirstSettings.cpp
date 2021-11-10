#include <string>
#include <fstream>
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


class AccuracyFirstSettings {
public:
	void configAccuracyFirst(CBarcodeReader *reader) {
		PublicRuntimeSettings settings;
		reader->GetRuntimeSettings(&settings);
		settings.barcodeFormatIds = BF_ONED;
		settings.barcodeFormatIds_2 = BF_NULL;
		settings.minResultConfidence = 30;
		settings.minBarcodeTextLength = 6;
		reader->UpdateRuntimeSettings(&settings);

	}
	void configAccuracyFirstByTemplate(CBarcodeReader * reader) {
		int ret = reader->InitRuntimeSettingsWithFile("AccuracyFirstTemplate.json", CM_OVERWRITE);
		if (ret != DBR_OK) {
			cout << "Error" << endl;
			exit(0);
		}
	}

	void outputResults(TextResultArray* barcodeResults, CBarcodeReader& reader) {
		if (barcodeResults != NULL && barcodeResults->resultsCount > 0)
		{
			// Process each result in a loop
			for (int i = 0; i < barcodeResults->resultsCount; ++i)
			{
				cout << "Result " << (i + 1) << ":" << endl;

				// Get format of each barcode
				if (barcodeResults->results[i]->barcodeFormat != BF_NULL)
					cout << "    Barcode Format: " << barcodeResults->results[i]->barcodeFormatString << endl;
				else
					cout << "    Barcode Format: " << barcodeResults->results[i]->barcodeFormatString_2 << endl;

				// Get text result of each barcode
				cout << "    Barcode Text: " << barcodeResults->results[i]->barcodeText << endl;
			}
		}
		else
		{
			cout << "No barcode detected." << endl;
		}

		// Free the memory allocated for text results
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
 	// Create an instance of Barcode Reader
	CBarcodeReader reader;
	AccuracyFirstSettings af;

	string fileName = "../../../../images/AllSupportedBarcodeTypes.png";

	cout << "Decode through PublicRuntimeSettings:" << endl;
	{
		af.configAccuracyFirst(&reader);
		reader.DecodeFile(fileName.c_str(), "");
		reader.GetAllTextResults(&barcodeResults);
		af.outputResults(barcodeResults, reader);
	}
	cout << endl;
	cout << "Decode through parameters template:" << endl;
	{
		af.configAccuracyFirstByTemplate(&reader);
		reader.DecodeFile(fileName.c_str(), "");
		reader.GetAllTextResults(&barcodeResults);
		af.outputResults(barcodeResults, reader);
	}
	cout << "Press any key to quit..." << endl;
    	cin.ignore();
    	return 0;
};









