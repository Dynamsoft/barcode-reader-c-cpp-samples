# Dynamsoft Barcode Reader Samples - C/C++ Edition

This repository contains multiple samples that demonstrates how to use the <a href="https://www.dynamsoft.com/barcode-reader/overview/?product=dbr&utm_source=samples&package=c_cpp" target="_blank">Dynamsoft Barcode Reader</a> C/C++ Edition.

## Requirements
- Operating systems:
  - Windows: 7, 8, 10, 2003, 2008, 2008 R2, 2012 and above.
  - Linux x64: Ubuntu 14.04.4+ LTS, Debian 8+, etc
  - Linux arm 32bit
  - Linux arm 64bit
- Developing Tool
  - Visual Studio 2008 or above
  - G++ 5.4+  
  - GCC 5.4+

## Samples

| C++ Sample | Description |
|---|---|
| [`HelloWorld`](samples/C++/HelloWorld) | This sample demonstrates the simplest way to read barcodes from an image file and output barcode format and text. |
| [`GeneralSettings`](samples/C++/GeneralSettings) | This sample demonstrates how to configure general used settings and read barcodes from an image file. |
| [`BatchDecode`](samples/C++/Performance/BatchDecode) | This sample demonstrates how to read barcodes from files in the specified directory and return results plus performance metrics. |
| [`VideoDecoding`](samples/C++/VideoDecoding/) | This sample demonstrates how to read barcodes from video frames. |
| [`SpeedFirstSettings`](samples/C++/Performance/SpeedFirstSettings) | This sample demonstrates how to configure Dynamsoft Barcode Reader to read barcodes as fast as possible. The downside is that read-rate and accuracy might be affected. |
| [`ReadRateFirstSettings`](samples/C++/Performance/ReadRateFirstSettings) | This sample demonstrates how to configure Dynamsoft Barcode Reader to read as many barcodes as possible at one time. The downside is that speed and accuracy might be affected. It is recommended to apply these configurations when decoding multiple barcodes from a single image. |
| [`AccuracyFirstSettings`](samples/C++/Performance/AccuracyFirstSettings) | This sample demonstrates how to configure Dynamsoft Barcode Reader to read barcodes as accurately as possible. The downside is that speed and read-rate might be affected. It is recommended to apply these configurations when misreading is unbearable. |
| [`MultiThreadDecoding`](samples/C++/MultiThreadDecoding) | This sample demonstrates how to read barcodes in multiple threads. |
| [`ReadLowQualityBarcodes`](samples/C++/UseCases/ReadLowQualityBarcodes) | This sample demonstrates how to use intermediate result to decode low quality barcodes. |

| C Sample | Description |
|---|---|
| [`HelloWorld`](samples/C/HelloWorld) | This sample demonstrates the simplest way to read barcodes from an image file and output barcode format and text. |
| [`GeneralSettings`](samples/C/GeneralSettings) | This sample demonstrates how to configure general used settings and read barcodes from an image file. |
## License

The library requires a license to work, you use the API InitLicense to initialize license key and activate the SDK.

These samples use a free public trial license which require network connection to function. You can request a 30-day free trial license key from <a href="https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp" target="_blank">Customer Portal</a> which works offline.

## Contact Us

<a href="https://www.dynamsoft.com/company/contact/">Contact Dynamsoft</a> if you have any questions.
