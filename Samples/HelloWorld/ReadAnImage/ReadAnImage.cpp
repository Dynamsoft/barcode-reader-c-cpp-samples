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
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#else
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#endif
#endif

int main()
{
	int errorCode = 1;
	char errorMsg[512];

	// Initialize license.
	// You can request and extend a trial license from https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", errorMsg, 512);

	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_CACHE_USED)
	{
		cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << errorMsg << endl;
	}
	else
	{
		CCaptureVisionRouter *cvr = new CCaptureVisionRouter;

		string imageFile = "../../../Images/GeneralBarcodes.png";
		CCapturedResult *result = cvr->Capture(imageFile.c_str(), CPresetTemplate::PT_READ_BARCODES);

		if (result->GetErrorCode() != 0)
		{
			cout << "Error: " << result->GetErrorCode() << "," << result->GetErrorString() << endl;
		}
		CDecodedBarcodesResult *barcodeResult = result->GetDecodedBarcodesResult();
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
				const CBarcodeResultItem *barcodeResultItem = barcodeResult->GetItem(j);
				cout << "Result " << j + 1 << endl;
				cout << "Barcode Format: " << barcodeResultItem->GetFormatString() << endl;
				cout << "Barcode Text: " << barcodeResultItem->GetText() << endl;
			}
		}
		if (barcodeResult)
			barcodeResult->Release();
		if (result)
			result->Release();
		delete cvr, cvr = NULL;
	}
	cout << "Press any key to quit..." << endl;
	cin.ignore();
	return 0;
}
