#include <iostream>
#include <string>

#include "../../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../../Include/DynamsoftUtility.h"

using namespace std;
using namespace dynamsoft::license;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;
using namespace dynamsoft::utility;

#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#else
#pragma comment(lib, "../../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#endif
#endif

class MyCapturedResultReceiver : public CCapturedResultReceiver
{
	virtual void OnDecodedBarcodesReceived(CDecodedBarcodesResult *pResult)
	{
		const CFileImageTag *tag = dynamic_cast<const CFileImageTag *>(pResult->GetOriginalImageTag());

		cout << "File: " << tag->GetFilePath() << endl;
		cout << "Page: " << tag->GetPageNumber() << endl;

		if (pResult->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
		{
			cout << "Warning: " << pResult->GetErrorCode() << ", " << pResult->GetErrorString() << endl;
		}
		else if (pResult->GetErrorCode() != EC_OK)
		{
			cout << "Error: " << pResult->GetErrorString() << endl;
		}
		int count = pResult->GetItemsCount();
		cout << "Decoded " << count << " barcodes" << endl;
		for (int i = 0; i < count; i++)
		{
			const CBarcodeResultItem *barcodeResultItem = pResult->GetItem(i);
			if (barcodeResultItem != NULL)
			{
				cout << "Result " << i + 1 << endl;
				cout << "Barcode Format: " << barcodeResultItem->GetFormatString() << endl;
				cout << "Barcode Text: " << barcodeResultItem->GetText() << endl;
			}
		}

		cout << endl;
	}
};

class MyImageSourceStateListener : public CImageSourceStateListener
{
private:
	CCaptureVisionRouter *m_router;

public:
	MyImageSourceStateListener(CCaptureVisionRouter *cvRouter)
	{
		m_router = cvRouter;
	}

	virtual void OnImageSourceStateReceived(ImageSourceState state)
	{
		if (state == ISS_EXHAUSTED)
			m_router->StopCapturing();
	}
};

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

		CDirectoryFetcher *fetcher = new CDirectoryFetcher;
		fetcher->SetDirectory("../../../Images");
		cvRouter->SetInput(fetcher);

		CCapturedResultReceiver *capturedReceiver = new MyCapturedResultReceiver;
		cvRouter->AddResultReceiver(capturedReceiver);

		CImageSourceStateListener *listener = new MyImageSourceStateListener(cvRouter);
		cvRouter->AddImageSourceStateListener(listener);

		errorCode = cvRouter->StartCapturing(CPresetTemplate::PT_READ_BARCODES, true, errorMsg, 512);
		if (errorCode != EC_OK)
			cout << "error:" << errorMsg << endl;

		delete cvRouter, cvRouter = NULL;
		delete fetcher, fetcher = NULL;
		delete listener, listener = NULL;
		delete capturedReceiver, capturedReceiver = NULL;
	}
	cout << "Press any key to quit..." << endl;
	cin.ignore();
	return 0;
}
