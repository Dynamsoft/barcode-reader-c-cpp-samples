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


## License

The library requires a license to work, you use the API InitLicenseFromDLS to tell the program where to fetch your license.
If the Organizaion ID is not specified or set as "200001", a 7-day (public) trial license will be used by default which is the case in these samples.
Note that network connection is required for this license to work.

When using your own license, locate the following line and specify your Organization ID.
// organizationID = "200001";

If you don't have a license yet, you can request a trial from <a href="https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp" target="_blank">Customer Portal</a>

For more information, please refer to https://www.dynamsoft.com/license-server/docs/about/licensefaq.html.

## Contact Us

https://www.dynamsoft.com/company/contact/
