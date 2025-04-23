#include <iostream>
#include <string>

#include "../../Include/DynamsoftCaptureVisionRouter.h"

using namespace std;
using namespace dynamsoft::license;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;
using namespace dynamsoft::basic_structures;
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#endif
#endif
int main()
{
	int errorCode = 1;
	char errorMsg[512];

	// 1. Initialize license.
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	// You can also request a 30-day trial license in the customer portal: https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", errorMsg, 512);

	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_CACHE_USED)
	{
		cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << errorMsg << endl;
	}
	else
	{
		// 2. Create an instance of CCaptureVisionRouter.
		CCaptureVisionRouter* cvRouter = new CCaptureVisionRouter;
		errorCode = cvRouter->InitSettingsFromFile("../../Dist/Templates/ReadDPM.json",errorMsg, 512);

        // 3. Replace with your own dpm barcode image path.
		string imageFile = "../../Images/DPM.png";

		// 4. Decode barcodes from the image file.
		CCapturedResult* result = cvRouter->Capture(imageFile.c_str(), "");

		if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
		{
			cout << "Warning: " << result->GetErrorCode() << ", " << result->GetErrorString() << endl;
		}
		else if (result->GetErrorCode() != ErrorCode::EC_OK)
		{
			cout << "Error: " << result->GetErrorCode() << "," << result->GetErrorString() << endl;
		}

		// 5. Output the barcode format and barcode text.
		CDecodedBarcodesResult* barcodeResult = result->GetDecodedBarcodesResult();
		if (barcodeResult == nullptr || barcodeResult->GetItemsCount() == 0)
		{
			cout << "No barcode found." << endl;
		}
		else
		{
			int barcodeResultItemCount = barcodeResult->GetItemsCount();
			cout << "Decoded " << barcodeResultItemCount << " barcodes" << endl;

			for (int j = 0; j < barcodeResultItemCount; j++)
			{
				const CBarcodeResultItem* barcodeResultItem = barcodeResult->GetItem(j);
				cout << "Barcode " << j + 1 << endl;
				cout << "Barcode Format: " << barcodeResultItem->GetFormatString() << endl;
				cout << "Barcode Text: " << barcodeResultItem->GetText() << endl;
			}
		}

		// 6. Release the barcode result.
		if (barcodeResult)
			barcodeResult->Release();

		// 7. Release the capture result.
		if (result)
			result->Release();

		// 8. Release the allocated memory.
		delete cvRouter, cvRouter = NULL;
	}
	cout << "Press any key to quit..." << endl;
	cin.ignore();
	return 0;
}