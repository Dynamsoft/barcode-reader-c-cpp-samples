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
		// Obtain current runtime settings of instance.
		PublicRuntimeSettings settings;
		reader->GetRuntimeSettings(&settings);

		// 1. Set expected barcode format
		// The more precise the barcode format is set, the higher the accuracy.
		// Mostly, misreading only occurs on reading oneD barcode. So here we use OneD barcode format to demonstrate.
		settings.barcodeFormatIds = BF_ONED;
		settings.barcodeFormatIds_2 = BF_NULL;

		// 2. Set the minimal result confidence.
		// The greater the confidence, the higher the accuracy.
		// Filter by minimal confidence of the decoded barcode. We recommend using 30 as the default minimal confidence value
		settings.minResultConfidence = 30;

		// 3. Sets the minimum length of barcode text for filtering.
		// The more precise the barcode text length is set, the higher the accuracy.
		settings.minBarcodeTextLength = 6;

		// Apply the new settings to the instance
		reader->UpdateRuntimeSettings(&settings);

	}
	void configAccuracyFirstByTemplate(CBarcodeReader * reader) {
		// Compared with PublicRuntimeSettings, parameter templates have a richer ability to control parameter details.
		// Please refer to the parameter explanation in "AccuracyFirstTemplate.json" to understand how to control accuracy first.
		int ret = reader->InitRuntimeSettingsWithFile("AccuracyFirstTemplate.json", CM_OVERWRITE);
		if (ret != DBR_OK) {
			cout << "Error" << endl;
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

	// Accuracy = The number of correctly decoded barcodes/the number of all decoded barcodes
	// There are two ways to configure runtime parameters. One is through PublicRuntimeSettings, the other is through parameters template.
	cout << "Decode through PublicRuntimeSettings:" << endl;
	{
		//	config through PublicRuntimeSettings
		af.configAccuracyFirst(&reader);
		
		//	Decode barcodes from an image file by current runtime settings. The second parameter value "" means to decode through the current PublicRuntimeSettings.
		reader.DecodeFile(fileName.c_str(), "");

		//	Get all barcode results
		reader.GetAllTextResults(&barcodeResults);

		//	Output the barcode format and barcode text.
		af.outputResults(barcodeResults, reader);
	}
	cout << endl;
	cout << "Decode through parameters template:" << endl;
	{
		//	config through parameters template
		af.configAccuracyFirstByTemplate(&reader);

		//	Decode barcodes from an image file by template.
		reader.DecodeFile(fileName.c_str(), "");

		//	Get all barcode results
		reader.GetAllTextResults(&barcodeResults);

		//	Output the barcode format and barcode text.
		af.outputResults(barcodeResults, reader);
	}
	cout << "Press any key to quit..." << endl;
    cin.ignore();
    return 0;
};









