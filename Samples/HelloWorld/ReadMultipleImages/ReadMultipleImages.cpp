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
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#else
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../../Distributables/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#endif
#endif

class MyCapturedResultReceiver : public CCapturedResultReceiver
{
	virtual void OnDecodedBarcodesReceived(CDecodedBarcodesResult *pResult)
	{
		const CFileImageTag *tag = dynamic_cast<const CFileImageTag *>(pResult->GetOriginalImageTag());

		cout << "File: " << tag->GetFilePath() << endl;

		if (pResult->GetErrorCode() != EC_OK)
		{
			cout << "Error: " << pResult->GetErrorString() << endl;
		}
		else
		{
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
		}

		cout << endl;
	}
};

class MyImageSourceStateListener : public CImageSourceStateListener
{
private:
	CCaptureVisionRouter *m_router;

public:
	MyImageSourceStateListener(CCaptureVisionRouter *router)
	{
		m_router = router;
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

		CDirectoryFetcher *fetcher = new CDirectoryFetcher;
		fetcher->SetDirectory("../../../Images");
		cvr->SetInput(fetcher);

		CCapturedResultReceiver *capturedReceiver = new MyCapturedResultReceiver;
		cvr->AddResultReceiver(capturedReceiver);

		CImageSourceStateListener *listener = new MyImageSourceStateListener(cvr);
		cvr->AddImageSourceStateListener(listener);

		errorCode = cvr->StartCapturing(CPresetTemplate::PT_READ_BARCODES, true, errorMsg, 512);
		if (errorCode != EC_OK)
			cout << "error:" << errorMsg << endl;

		delete cvr, cvr = NULL;
		delete fetcher, fetcher = NULL;
		delete listener, listener = NULL;
		delete capturedReceiver, capturedReceiver = NULL;
	}
	cout << "Press any key to quit..." << endl;
	cin.ignore();
	return 0;
}
