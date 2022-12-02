// GeneralSettings.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
    char szErrorMsg[512];
    PublicRuntimeSettings settings;
    TextResultArray* barcodeResults = NULL;

    // 1.Initialize license.
    // The string "DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9" here is a free public trial license. Note that network connection is required for this license to work.
    // If you don't have a license yet, you can request a trial from https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp 
    errorCode = CBarcodeReader::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", szErrorMsg, 512);
    if (errorCode != DBR_OK)
    {
        cout << szErrorMsg << endl;
    }

    // 2. Create an instance of Barcode Reader
    CBarcodeReader dbr;

    // 3. Configure settings

    // 3.1 Through PublicRuntimeSetting

    // 3.1.1 Call GetRuntimeSettings to get current runtime settings. 
    dbr.GetRuntimeSettings(&settings);

    // 3.1.2 Configure one or more specific settings
    // In this sample, we configure three settings: 
    // try to finnd PDF 417 and DotCode
        // The barcode format our library will search for is composed of BarcodeFormat group 1 and BarcodeFormat group 2.
        // So you need to specify the barcode format in group 1 and group 2 individually.
    settings.barcodeFormatIds = BF_PDF417;
    settings.barcodeFormatIds_2 = BF2_DOTCODE;
    // try to find 2 barcodes
    settings.expectedBarcodesCount = 2;
    // try to find barcodes in the lower part of the image
    settings.region.regionLeft = 0;
    settings.region.regionRight = 100;
    settings.region.regionTop = 50;
    settings.region.regionBottom = 100;
    settings.region.regionMeasuredByPercentage = 1;

    // 3.1.3 Call UpdateRuntimeSettings to apply above settings
    dbr.UpdateRuntimeSettings(&settings, szErrorMsg, 512);

    // 3.2 Through JSON template
    //dbr.InitRuntimeSettingsWithString("{\"ImageParameter\":{\"Name\":\"S1\",\"RegionDefinitionNameArray\":[\"R1\"]},\"RegionDefinition\":{\"Name\":\"R1\",\"BarcodeFormatIds\":[\"BF_PDF417\"],\"BarcodeFormatIds_2\":[\"BF2_POSTALCODE\"],\"ExpectedBarcodesCount\":2,\"Left\":0,\"Right\":100,\"Top\":50,\"Bottom\":100,\"MeasuredByPercentage\":1}}", CM_IGNORE, szErrorMsg, 512);

    // 4. Read barcode from an image file
    errorCode = dbr.DecodeFile("../../../images/AllSupportedBarcodeTypes.png", "");
    if (errorCode != DBR_OK)
        cout << CBarcodeReader::GetErrorString(errorCode) << endl;

    // 5. Get all barcode results
    dbr.GetAllTextResults(&barcodeResults);


    if (barcodeResults != NULL && barcodeResults->resultsCount > 0)
    {
        // Process each result in a loop
        for (int i = 0; i < barcodeResults->resultsCount; ++i)
        {
            cout << "Result " << (i + 1) << ":" << endl;

            // 5.1. Get format of each barcode
            cout << "    Barcode Format: " << barcodeResults->results[i]->barcodeFormatString << endl;


            // 5.2. Get text result of each barcode
            cout << "    Barcode Text: " << barcodeResults->results[i]->barcodeText << endl;
        }
    }
    else
    {
        cout << "No barcode detected." << endl;
    }

    // 6. Free the memory allocated for text results
    if (barcodeResults != NULL)
        CBarcodeReader::FreeTextResults(&barcodeResults);

    cout << "Press any key to quit..." << endl;
    cin.ignore();
    return 0;
}

