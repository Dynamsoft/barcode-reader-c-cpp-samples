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

class ReadDPMBarcode
{
public:
	void configReadDPMBarcode(CBarcodeReader *reader)
	{
		// Obtain current runtime settings of instance.
		PublicRuntimeSettings settings;
		reader->GetRuntimeSettings(&settings);

		// Set expected barcode formats.
		// Generally, the most common dpm barcode is datamatrix or qrcode
		settings.barcodeFormatIds = BF_DATAMATRIX | BF_QR_CODE;

		// Set grayscale transformation modes.
		// DPM barcodes may be dark and printed on the light background, or it may be light and printed on the dark background.
		// By default, the library can only locate the dark barcodes that stand on a light background. "GTM_INVERTED":The image will be transformed into inverted grayscale.
		settings.furtherModes.grayscaleTransformationModes[0] = GTM_ORIGINAL;
		settings.furtherModes.grayscaleTransformationModes[1] = GTM_INVERTED;

		// Enable dpm modes.
		// It is a parameter to control how to read direct part mark(DPM) barcodes.
		settings.furtherModes.dpmCodeReadingModes[0] = DPMCRM_GENERAL;

		// Apply the new settings to the instance
		reader->UpdateRuntimeSettings(&settings);

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

int main()
{
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
	ReadDPMBarcode rb;

	string fileName = "../../../../images/dpm.jpg";

	//	config through ReadDPM
	rb.configReadDPMBarcode(&reader);

	//	Decode barcodes from an image file by current runtime settings. The second parameter value "" means to decode through the current PublicRuntimeSettings.
	reader.DecodeFile(fileName.c_str(), "");

	//	Get all barcode results
	reader.GetAllTextResults(&barcodeResults);

	//	Output the barcode format and barcode text.
	rb.outputResults(barcodeResults, reader);

	cout << "Press any key to quit..." << endl;
	cin.ignore();
	return 0;
}