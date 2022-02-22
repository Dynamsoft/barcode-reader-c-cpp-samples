// HelloWorld.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

// Include headers of DynamsoftBarcodeReader SDK
#include "../../../include/DynamsoftBarcodeReader.h"
#include "../../../include/DynamsoftCommon.h"

using namespace std;
using namespace dynamsoft::dbr;

// Link libraries of DynamsoftBarcodeReader SDK for Windows.
#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../../lib/Windows/x64/DBRx64.lib")
#else
#pragma comment(lib, "../../../lib/Windows/x86/DBRx86.lib")
#endif
#endif

int main()
{
    int errorCode = 0;
    char szErrorMsg[256];
    TextResultArray* barcodeResults = NULL;

    // 1. Initialize license
    /*
    // By setting organization ID as "200001", a free public trial license will be used for license verification.
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

    // 2. Create an instance of Barcode Reader
    CBarcodeReader dbr;

    // 3. Read barcode from an image file
    errorCode = dbr.DecodeFile("../../../images/AllSupportedBarcodeTypes.png", "");
    if (errorCode != DBR_OK)
        cout << CBarcodeReader::GetErrorString(errorCode) << endl;

    // 4. Get all barcode results
    dbr.GetAllTextResults(&barcodeResults);

    if (barcodeResults != NULL && barcodeResults->resultsCount > 0)
    {
        // Process each result in a loop
        for (int i = 0; i < barcodeResults->resultsCount; ++i)
        {
            cout << "Result " << (i + 1) << ":" << endl;

            // 4.1. Get format of each barcode
            cout << "    Barcode Format: " << barcodeResults->results[i]->barcodeFormatString << endl;

            // 4.2. Get text result of each barcode
            cout << "    Barcode Text: " << barcodeResults->results[i]->barcodeText << endl;
        }
    }
    else
    {
        cout << "No barcode detected." << endl;
    }

    // 5. Free the memory allocated for text results
    if (barcodeResults != NULL)
        CBarcodeReader::FreeTextResults(&barcodeResults);

    cout << "Press any key to quit..." << endl;
    cin.ignore();
    return 0;
}

