// GeneralSettings.c : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>

// Include headers of DynamsoftBarcodeReader SDK
#include "../../../include/DynamsoftBarcodeReader.h"
#include "../../../include/DynamsoftCommon.h"


// Link libraries of DynamsoftBarcodeReader SDK for Windows.
#if defined(_WIN64) || defined(_WIN32)
    #ifdef _WIN64
        #pragma comment(lib, "../../../lib/Windows/x64/DBRx64.lib")
    #else
        #pragma comment(lib, "../../../lib/Windows/x86/DBRx86.lib")
    #endif
#endif

void main()
{
    void* dbr = NULL;
    int errorCode = 0;
    char szErrorMsg[256];
    PublicRuntimeSettings settings;
    TextResultArray* barcodeResults = NULL;
    int index;

    // 1.Initialize license.
    // The string "DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9" here is a free public trial license. Note that network connection is required for this license to work.
    // If you don't have a license yet, you can request a trial from https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp 
    errorCode = DBR_InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", szErrorMsg, 256);
    if (errorCode != DBR_OK)
    {
        printf("%s\r\n", szErrorMsg);
    }

    // 2. Create an instance of Barcode Reader
    dbr = DBR_CreateInstance();

    // 3. Configure settings

    // 3.1 Through PublicRuntimeSetting

    // 3.1.1 Call DBR_GetRuntimeSettings to get current runtime settings. 
    DBR_GetRuntimeSettings(dbr, &settings);

    // 3.1.2 Configure one or more specific settings
    // In this sample, we configure three settings: 
    // try to finnd PDF 417 and DotCode
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

    // 3.1.3 Call DBR_UpdateRuntimeSettings to apply above settings
    DBR_UpdateRuntimeSettings(dbr, &settings, szErrorMsg, 256);

    // 3.2 Through JSON template
    //DBR_InitRuntimeSettingsWithString(dbr, "{\"ImageParameter\":{\"Name\":\"S1\",\"RegionDefinitionNameArray\":[\"R1\"]},\"RegionDefinition\":{\"Name\":\"R1\",\"BarcodeFormatIds\":[\"BF_PDF417\"],\"BarcodeFormatIds_2\":[\"BF2_POSTALCODE\"],\"ExpectedBarcodesCount\":2,\"Left\":0,\"Right\":100,\"Top\":50,\"Bottom\":100,\"MeasuredByPercentage\":1}}", CM_IGNORE, szErrorMsg, 256);

    // 4. Read barcode from an image file
    errorCode = DBR_DecodeFile(dbr, "../../../images/AllSupportedBarcodeTypes.png", "");
    if (errorCode != DBR_OK)
        printf("%s\r\n", DBR_GetErrorString(errorCode));

    // 5. Get all barcode results
    DBR_GetAllTextResults(dbr, &barcodeResults);


    if (barcodeResults != NULL && barcodeResults->resultsCount > 0)
    {
        // Process each result in a loop
        for (index = 0; index < barcodeResults->resultsCount; ++index)
        {
            printf("Result %d:\r\n", (index + 1));

            // 5.1. Get format of each barcode
            if (barcodeResults->results[index]->barcodeFormat != BF_NULL)
                printf("    Barcode Format: %s\r\n", barcodeResults->results[index]->barcodeFormatString);
            else
                printf("    Barcode Format: %s\r\n", barcodeResults->results[index]->barcodeFormatString_2);

            // 5.2. Get text result of each barcode
            printf("    Barcode Text: %s\r\n", barcodeResults->results[index]->barcodeText);
        }
    }
    else
    {
        printf("No barcode detected.");
    }

    // 6. Free the memory allocated for text results
    if (barcodeResults != NULL)
        DBR_FreeTextResults(&barcodeResults);
    DBR_DestroyInstance(dbr);

    printf("Press any key to quit..." );
    getchar();
}

