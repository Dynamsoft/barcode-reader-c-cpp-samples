#include <iostream>
#include <string>

#include "../../../Include/DynamsoftCaptureVisionRouter.h"

using namespace std;
using namespace dynamsoft::license;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;
using namespace dynamsoft::basic_structures;
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#else
#pragma comment(lib, "../../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#endif
#endif

int main()
{
	int errorCode = 1;
	char errorMsg[512];

	// Initialize license.
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	// You can also request a 30-day trial license in the customer portal: https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", errorMsg, 512);

	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_WARNING)
	{
		cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << errorMsg << endl;
	}
	else
	{
		CCaptureVisionRouter *cvRouter = new CCaptureVisionRouter;

		string imageFile = "../../../Images/GeneralBarcodes.png";
		CCapturedResultArray* resultArray = cvRouter->CaptureMultiPages(imageFile.c_str(), CPresetTemplate::PT_READ_BARCODES);
		int count = resultArray->GetResultsCount();
		for (int i = 0; i < count; ++i)
		{
			const CCapturedResult* result = resultArray->GetResult(i);


			if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
			{
				cout << "Warning: " << result->GetErrorCode() << ", " << result->GetErrorString() << endl;
			}
			else if (result->GetErrorCode() != ErrorCode::EC_OK)
			{
				cout << "Error: " << result->GetErrorCode() << "," << result->GetErrorString() << endl;
			}
			CDecodedBarcodesResult* barcodeResult = result->GetDecodedBarcodesResult();
			if (barcodeResult == nullptr || barcodeResult->GetItemsCount() == 0)
			{
				cout << "No barcode found in page " << i + 1 << endl;
			}
			else
			{
				int barcodeResultItemCount = barcodeResult->GetItemsCount();
				cout << "Page " << i + 1 << " decoded " << barcodeResultItemCount << " barcodes" << endl;

				for (int j = 0; j < barcodeResultItemCount; j++)
				{
					const CBarcodeResultItem* barcodeResultItem = barcodeResult->GetItem(j);
					cout << "Result " << i + 1 << "-" << j + 1 << endl;
					cout << "Barcode Format: " << barcodeResultItem->GetFormatString() << endl;
					cout << "Barcode Text: " << barcodeResultItem->GetText() << endl;
				}
			}
			if (barcodeResult)
				barcodeResult->Release();
		}
		if (resultArray)
			resultArray->Release();
		delete cvRouter, cvRouter = NULL;
	}
	cout << "Press any key to quit..." << endl;
	cin.ignore();
	return 0;
}
