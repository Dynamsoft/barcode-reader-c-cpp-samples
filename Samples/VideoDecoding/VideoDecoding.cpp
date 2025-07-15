#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>

// Include headers of DynamsoftCaptureVisionRouter SDK
#include "../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../Include/DynamsoftUtility.h"
using namespace std;
using namespace dynamsoft::license;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;
using namespace dynamsoft::utility;
using namespace cv;
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#endif
#endif

class MyCapturedResultReceiver : public CCapturedResultReceiver
{

	virtual void OnDecodedBarcodesReceived(CDecodedBarcodesResult *pResult) override
	{
		if (pResult->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
		{
			cout << "Warning: " << pResult->GetErrorCode() << ", " << pResult->GetErrorString() << endl;
		}
		else if (pResult->GetErrorCode() != EC_OK)
		{
			cout << "Error: " << pResult->GetErrorString() << endl;
		}
		auto tag = pResult->GetOriginalImageTag();
		if (tag)
			cout << "ImageID:" << tag->GetImageId() << endl;
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

class MyVideoFetcher : public CImageSourceAdapter
{
public:
	MyVideoFetcher(){};
	~MyVideoFetcher(){};
	bool HasNextImageToFetch() const override
	{
		return true;
	}
	void MyAddImageToBuffer(const CImageData *img, bool bClone = true)
	{
		AddImageToBuffer(img, bClone);
	}
};

int main()
{
	cout << "Opening camera..." << endl;
	VideoCapture capture(0); // open the first camera
	if (!capture.isOpened())
	{
		cerr << "ERROR: Can't initialize camera capture" << endl;
		cout << "Press any key to quit..." << endl;
		cin.ignore();
		return 1;
	}
	int iRet = -1;
	char szErrorMsg[256];
	// Initialize license.
	// The string "DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9" here is a free public trial license. Note that network connection is required for this license to work.
	// You can also request a 30-day trial license in the customer portal: https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
	iRet = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", szErrorMsg, 256);
	if (iRet != ErrorCode::EC_OK && iRet != ErrorCode::EC_LICENSE_WARNING)
	{
		cout << "License initialization failed: ErrorCode: " << iRet << ", ErrorString: " << szErrorMsg << endl;
	}
	else
	{
		int errorCode = 1;
		char errorMsg[512] = {0};

		CCaptureVisionRouter *cvRouter = new CCaptureVisionRouter;

		MyVideoFetcher *fetcher = new MyVideoFetcher();
		fetcher->SetMaxImageCount(100);
		fetcher->SetBufferOverflowProtectionMode(BOPM_UPDATE);
		fetcher->SetColourChannelUsageType(CCUT_AUTO);
		cvRouter->SetInput(fetcher);

		CMultiFrameResultCrossFilter *filter = new CMultiFrameResultCrossFilter;
		filter->EnableResultCrossVerification(CRIT_BARCODE, true);
		filter->EnableResultDeduplication(CRIT_BARCODE, true);
		filter->SetDuplicateForgetTime(CRIT_BARCODE, 5000);
		cvRouter->AddResultFilter(filter);

		CCapturedResultReceiver *capturedReceiver = new MyCapturedResultReceiver;
		cvRouter->AddResultReceiver(capturedReceiver);

		errorCode = cvRouter->StartCapturing(CPresetTemplate::PT_READ_BARCODES, false, errorMsg, 512);
		if (errorCode != EC_OK)
		{
			cout << "error:" << errorMsg << endl;
		}
		else
		{
			for (int i = 1;; ++i)
			{
				auto start = std::chrono::high_resolution_clock::now();
				Mat frame;
				capture.read(frame);
				if (frame.empty())
				{
					cerr << "ERROR: Can't grab camera frame." << endl;
					break;
				}
				CFileImageTag tag(nullptr, 0, 0);
				tag.SetImageId(i);
				CImageData data(frame.rows * frame.step.p[0],
								frame.data,
								capture.get(CAP_PROP_FRAME_WIDTH),
								capture.get(CAP_PROP_FRAME_HEIGHT),
								frame.step.p[0],
								IPF_RGB_888,
								0,
								&tag);
				fetcher->MyAddImageToBuffer(&data);
				imshow("Frame", frame);
				auto finish = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
				int usingTime = elapsed.count();
				int key = 1;
				if (usingTime < 40)
					key = 40 - usingTime;
				key = waitKey(key);
				if (key == 27 /*ESC*/)
					break;
			}
			cvRouter->StopCapturing(false, true);
		}

		capture.release();

		delete cvRouter, cvRouter = NULL;
		delete fetcher, fetcher = NULL;
		delete filter, filter = NULL;
		delete capturedReceiver, capturedReceiver = NULL;
	}
	cout << "Press any key to quit..." << endl;
	cin.ignore();
	return 0;
}
