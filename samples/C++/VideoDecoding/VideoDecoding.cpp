#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

// Include headers of DynamsoftBarcodeReader SDK
#include "../../../include/DynamsoftBarcodeReader.h"
#include "../../../include/DynamsoftCommon.h"

using namespace cv;
using namespace std;
using std::cerr; 
using std::endl;
using std::cin;

// Link libraries of DynamsoftBarcodeReader SDK for Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../lib/Windows/x64/DBRx64.lib")
#else
#pragma comment(lib, "../../../lib/Windows/x86/DBRx86.lib")
#endif
#endif

using namespace std;
using namespace dynamsoft::dbr;


/// <summary>
/// This is the callback function to process the decoded barcode result
/// It will be triggled when barcodes are detected in a video frame
/// </summary>
void textResultCallback(int frameId, TextResultArray * barcodeResults, void * pUser)
{

	cout << endl << "frame = " << frameId << ", " << barcodeResults->resultsCount << " Total barcode(s) found: " << endl;

	for (int iIndex = 0; iIndex < barcodeResults->resultsCount; iIndex++)
	{
		cout << "Result " << (iIndex + 1) << ":" << endl;

		// Get format of each barcode
		cout << "    Barcode Format: " << barcodeResults->results[iIndex]->barcodeFormatString << endl;

		// Get text result of each barcode
		cout << "    Barcode Text: " << barcodeResults->results[iIndex]->barcodeText << endl;

	}

	// 5. Free the memory allocated for text results
	CBarcodeReader::FreeTextResults(&barcodeResults);
}

/// <summary>
/// This is the callback function to process the error returned
/// It will be triggled when error occurs when decoding a video frame
/// </summary>
void errorCallback(int frameId, int errorCode, void* pUser)
{
	if (errorCode != DBR_OK)
	{
		cout << endl << "frame = " << frameId << ", failed to read barcode: " << CBarcodeReader::GetErrorString(errorCode) << endl;
		return;
	}
}




int main()
{
	Mat frame;
	cout << "Opening camera..." << endl;
	VideoCapture capture(0); // open the first camera
	if (!capture.isOpened())
	{
		cerr << "ERROR: Can't initialize camera capture" << endl;
		//return 1;
	}
	 
	int iRet = -1;
	char szErrorMsg[256];
	// Initialize license.
  	// The string "DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9" here is a free public trial license. Note that network connection is required for this license to work.
  	// If you don't have a license yet, you can request a trial from https://www.dynamsoft.com/customer/license/trialLicense?architecture=dcv&product=dbr&utm_source=samples&package=c_cpp 
   	iRet = CBarcodeReader::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", szErrorMsg, 256);
	if (iRet != DBR_OK)
	{
		cout << szErrorMsg << endl;
	}

	// Create an instance of Barcode Reader
	CBarcodeReader* dbr = CBarcodeReader::GetInstance();
	if (dbr == NULL)
	{
		cout << "Get instance failed." << endl;
		return -1;
	}
	// Set the callback functions to process the barcode result and error returned by Dynamsoft Barcode Reader SDK
	dbr->SetTextResultCallback(textResultCallback, NULL);
	dbr->SetErrorCallback(errorCallback, NULL);

	// Initialize a group of parameters for video decoding
	FrameDecodingParameters parameters;
	dbr->InitFrameDecodingParameters(&parameters);
	capture >> frame;
	parameters.width = capture.get(CAP_PROP_FRAME_WIDTH);
	parameters.height = capture.get(CAP_PROP_FRAME_HEIGHT);
	parameters.stride = frame.step.p[0];
	parameters.imagePixelFormat = IPF_RGB_888;
	//parameters.maxQueueLength = 10;
	//parameters.maxResultQueueLength = 10;
	//parameters.region.regionMeasuredByPercentage = 1;
	//parameters.region.regionTop = 0;
	//parameters.region.regionBottom = 100;
	//parameters.region.regionLeft = 0;
	//parameters.region.regionRight = 100;
	//parameters.threshold =0.01;
	//parameters.fps = 0;

	// Start a new thread to decode barcodes from the inner frame queue
	iRet = dbr->StartFrameDecodingEx(parameters, "");
	
	for(;;)
	{
		// Read the next frame from camera
		capture >> frame; 
		if (frame.empty())
		{
			cerr << "ERROR: Can't grab camera frame." << endl;
			break;
		}

		// Append a frame image buffer to the inner frame queue
		dbr->AppendFrame(frame.data);

		imshow("Frame", frame);

		int key = waitKey(40);
		if (key == 27/*ESC*/)
			break;

	}

	// Stop the frame decoding thread created by StartFrameDecodingEx
	dbr->StopFrameDecoding();

	dbr->Recycle();
	dbr = NULL;
	return 0;
}
