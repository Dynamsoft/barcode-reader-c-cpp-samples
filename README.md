# Dynamsoft Barcode Reader Samples - C++ Edition

This repository contains multiple samples that demonstrates how to use the <a href="https://www.dynamsoft.com/barcode-reader/overview/?product=dbr&utm_source=samples&package=c_cpp" target="_blank">Dynamsoft Barcode Reader</a> C++ Edition.

## Requirements
- Operating systems:
  - Windows: Windows 8 and higher, or Windows Server 2012 and higher.
  - Linux x64: Ubuntu 14.04.4+ LTS, Debian 8+, CentOS 7+, etc
  - Linux ARM 64-bit
- Developing Tool
  - Visual Studio 2012 or above
  - G++ 5.4+  
  - GCC 5.4+

## Samples

### Basic Barcode Reader Samples

| Sample | Description |
|---|---|
| [`ReadAnImage`](Samples/HelloWorld/ReadAnImage) | This sample demonstrates the simplest way to read barcodes from an image file and output barcode format and text. |
| [`ReadMultipleImages`](Samples/HelloWorld/ReadMultipleImages) | This sample demonstrates the simplest way to read barcodes from directory with image files and output barcode format and text. |
| [`VideoDecoding`](Samples/VideoDecoding) | This sample demonstrates how to read barcodes from video frames. |
| [`MultiThreadDecoding`](Samples/MultiThreadDecoding) | This sample demonstrates how to read barcodes and get barcode results in self-managed thread. |
| [`BarcodePerformanceEvaluator`](Samples/BarcodePerformanceEvaluator) | This sample demonstrates how to read barcodes from files in the specified directory and return results plus performance metrics. |
| [`GeneralSettings`](Samples/GeneralSettings) | This sample demonstrates how to configure general used settings and read barcodes from an image file. |
| [`ReadDPMBarcode`](Samples/ReadDPMBarcode) | This sample demonstrates how to read DPM (Direct Part Marking) barcodes and get barcode results. |
| [`ParameterTuner`](Samples/ParameterTuner) | This sample demonstrates how to adjust and test different parameter settings to optimize barcode recognition performance. |
| [`ShowLocalizedVSDecodedBarcodes`](Samples/ShowLocalizedVSDecodedBarcodes) | This sample demonstrates how to highlight successfully decoded and only-localized barcodes with different styles of rectangles. |

### Additional Samples using Capture Vision SDK

In addition to the classic barcode decoding samples listed above, the following samples go a step further by parsing the decoded results and showcasing more structured workflows.

> [!IMPORTANT]
> These samples use the `dynamsoft-capture-vision` package instead of `dynamsoft-barcode-reader`. If you're switching to these samples, make sure to [download](https://www.dynamsoft.com/capture-vision/confirmation/#desktop) and use the correct package.

| Sample | Description |
| --- | --- |
| [`DriverLicenseScanner`](https://github.com/Dynamsoft/capture-vision-cpp-samples/blob/main/Samples/DriverLicenseScanner) | Shows how to capture and extract user's information from driver license/ID. |
| [`VINScanner`](https://github.com/Dynamsoft/capture-vision-cpp-samples/blob/main/Samples/VINScanner) | Shows how to capture and extract vehicle's information from Vehicle Identification Number (VIN). |
| [`GS1AIScanner`](https://github.com/Dynamsoft/capture-vision-cpp-samples/blob/main/Samples/GS1AIScanner) | Shows how to extract and interpret GS1 Application Identifiers (AIs) from GS1 barcodes. |

## License

The library requires a license to work, you use the API InitLicense to initialize license key and activate the SDK.

These samples use a free public trial license which require network connection to function. You can request a 30-day free trial license key from <a href="https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp" target="_blank">Customer Portal</a> which works offline.

## Contact Us

<a href="https://www.dynamsoft.com/company/contact/">Contact Dynamsoft</a> if you have any questions.
