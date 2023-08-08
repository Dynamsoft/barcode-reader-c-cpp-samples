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
#pragma comment(lib, "../../../Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../../Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#else
#pragma comment(lib, "../../../Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../../Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
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

	if (errorCode != EC_OK)
		cout << "License initialization error: " << errorMsg << endl;

	CCaptureVisionRouter* cvr = new CCaptureVisionRouter;

	string imageFile = "../../../Images/AllSupportedBarcodeTypes.png";
	CCapturedResult* result = cvr->Capture(imageFile.c_str(), CPresetTemplate::PT_READ_BARCODES);

	if (result->GetErrorCode() != 0) {
		cout << "Error: " << result->GetErrorCode() << "," << result->GetErrorString() << endl;
	}

	int capturedResultItemCount = result->GetItemsCount();
	cout << "Decoded " << capturedResultItemCount << " barcodes" << endl;

	for (int j = 0; j < capturedResultItemCount; j++) 
	{
		const CCapturedResultItem* capturedResultItem = result->GetItem(j);
		CapturedResultItemType type = capturedResultItem->GetType();
		if (type == CapturedResultItemType::CRIT_BARCODE) 
		{
			const CBarcodeResultItem* barcodeResultItem = dynamic_cast<const CBarcodeResultItem*> (capturedResultItem);
			cout << "Result " << j + 1 << endl;
			cout << "Barcode Format: " << barcodeResultItem->GetFormatString() << endl;
			cout << "Barcode Text: " << barcodeResultItem->GetText() << endl;
		}
	}

	delete cvr, cvr = NULL;
	delete result, result = NULL;

	cout << "Press any key to quit..." << endl;
	cin.ignore();
	return 0;
}

