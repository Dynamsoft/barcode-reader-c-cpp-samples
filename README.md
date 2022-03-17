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

| Sample | Description |
|---|---|
| HelloWorld | This sample demonstrates the simplest way to read barcodes from an image file and output barcode format and text. |
| GeneralSettings | This sample demonstrates how to configure general used settings and read barcodes from an image file. |
| BatchDecode | This sample demonstrates how to read barcodes from files in the specified directory and return results plus performance metrics. |
| VideoDecoding | This sample demonstrates how to read barcodes from video frames. |
| SpeedFirstSettings | This sample demonstrates how to configure Dynamsoft Barcode Reader to read barcodes as fast as possible. The downside is that read-rate and accuracy might be affected. |
| ReadRateFirstSettings | This sample demonstrates how to configure Dynamsoft Barcode Reader to read as many barcodes as possible at one time. The downside is that speed and accuracy might be affected. It is recommended to apply these configurations when decoding multiple barcodes from a single image. |
| AccuracyFirstSettings | This sample demonstrates how to configure Dynamsoft Barcode Reader to read barcodes as accurately as possible. The downside is that speed and read-rate might be affected. It is recommended to apply these configurations when misreading is unbearable. |


## License

The library requires a license to work, you use the API InitLicense to initialize license key and activate the SDK.

These samples use a <a href="https://www.dynamsoft.com/license-server/docs/about/terms.html?ver=latest&product=dbr&utm_source=samples&package=c_cpp#public-trial-license" target="_blank">free public trial license</a>. Note that network connection is required for this license to work.

You can also request a 30-day trial license from <a href="https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp" target="_blank">Customer Portal</a>

For more information, please refer to https://www.dynamsoft.com/license-server/docs/about/licensefaq.html.

## Contact Us

https://www.dynamsoft.com/company/contact/
