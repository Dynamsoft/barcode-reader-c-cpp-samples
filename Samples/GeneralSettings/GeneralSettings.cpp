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

	// Initialize license.
	// The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
	// You can also request a 30-day trial license in the customer portal: https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
	errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", errorMsg, 512);

	if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_CACHE_USED)
	{
		cout << "License initialization failed: ErrorCode: " << errorCode << ", ErrorString: " << errorMsg << endl;
	}
	else
	{
        // 2. Create an instance of CaptureVisionRouter.
		CCaptureVisionRouter *cvr = new CCaptureVisionRouter;

		// 3. General settings (including barcode format, barcode count and scan region) through SimplifiedCaptureVisionSettings
        // 3.1 Obtain current runtime settings of instance.
		SimplifiedCaptureVisionSettings settings;
		errorCode = cvr->GetSimplifiedSettings(CPresetTemplate::PT_READ_BARCODES, &settings);
		if (errorCode != ErrorCode::EC_OK)
		{
			cout << "Get simplified settings failed, Error: " << errorCode << endl;
		}
		else
		{
			// 3.2 Set the expected barcode format you want to read.
			settings.barcodeSettings.barcodeFormatIds = BarcodeFormat::BF_QR_CODE | BarcodeFormat::BF_CODE_128;

			// 3.3 Set the expected barcode count you want to read. 
			settings.barcodeSettings.expectedBarcodesCount = 10;

			// 3.4 Set the grayscale transformation modes.
			settings.barcodeSettings.grayscaleTransformationModes[0] = GrayscaleTransformationMode::GTM_AUTO;
			
			// 3.5 Set the ROI(region of interest) to speed up the barcode reading process. 
            // Note: DBR supports setting coordinates by pixels or percentages. The origin of the coordinate system is the upper left corner point.
			settings.roiMeasuredInPercentage = 1;
			settings.roi.points[0] = CPoint(0, 0);
			settings.roi.points[1] = CPoint(100, 0);
			settings.roi.points[2] = CPoint(100, 100);
			settings.roi.points[3] = CPoint(0, 100);

			// 3.6 Apply the new settings to the instance.
			errorCode = cvr->UpdateSettings(CPresetTemplate::PT_READ_BARCODES, &settings, errorMsg, 512);
			if (errorCode != ErrorCode::EC_OK)
				cout << "Update settings failed: ErrorCode: " << errorCode << ", ErrorString : " << errorMsg << endl;
			
			// 4. Replace by your own image path
			string imageFile = "../../Images/GeneralBarcodes.png";

			// 5. Decode barcodes from the image file.
			CCapturedResult *result = cvr->Capture(imageFile.c_str(), CPresetTemplate::PT_READ_BARCODES);

			if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
			{
				cout << "Warning: " << result->GetErrorCode() << ", " << result->GetErrorString() << endl;
			}
			else if (result->GetErrorCode() != ErrorCode::EC_OK)
			{
				cout << "Error: " << result->GetErrorCode() << "," << result->GetErrorString() << endl;
			}

			// 6. Output the barcode text.
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

			// 7. Release the barcode result.
			if (barcodeResult)
				barcodeResult->Release();

			// 8. Release the capture result.
			if (result)
				result->Release();
			
			// 9. Release the allocated memory.
			delete cvr, cvr = NULL;
		}
	}
	cout << "Press any key to quit..." << endl;
	cin.ignore();
	return 0;
}