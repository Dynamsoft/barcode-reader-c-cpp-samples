#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <set>
#include "../../Include/DynamsoftCaptureVisionRouter.h"
#include "../../Include/DynamsoftUtility.h"
using namespace dynamsoft;
using namespace dynamsoft::cvr;
using namespace dynamsoft::license;
using namespace dynamsoft::intermediate_results;
using namespace dynamsoft::utility;
using namespace dynamsoft::dbr;
#ifndef MYMAX
#define MYMAX(a,b)  (((a) > (b)) ? (a) : (b))
#endif // !MYMAX

#if defined(_WIN64) || defined(_WIN32)
#ifdef _WIN64
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCaptureVisionRouterx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftCorex64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftUtilityx64.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x64/DynamsoftLicensex64.lib")
#else
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCaptureVisionRouterx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftCorex86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftUtilityx86.lib")
#pragma comment(lib, "../../Dist/Lib/Windows/x86/DynamsoftLicensex86.lib")
#endif
#endif

class MyIntermediateResultReceiver : public CIntermediateResultReceiver {
public:
    std::vector<CQuadrilateral> locations;
    void OnLocalizedBarcodesReceived(dbr::intermediate_results::CLocalizedBarcodesUnit* result, const IntermediateResultExtraInfo* info) override {
        if (info->isSectionLevelResult) {
            for (int i = 0; i < result->GetCount(); ++i) {
                locations.push_back(result->GetLocalizedBarcode(i)->GetLocation());
            }
        }
    }
};

bool isWithin10Percent(double area1, double area2) {
    if (area1 == 0 || area2 == 0) return false;
    double ratio = abs(area1 - area2) / MYMAX(area1, area2);
    return ratio <= 0.10;
}

bool seemAsSameLocation(const CQuadrilateral& location, const CQuadrilateral& resultLoc) {
    CPoint centralPoint(0, 0);
    for (const auto& point : location.points) {
        centralPoint[0] += point[0];
        centralPoint[1] += point[1];
    }
    centralPoint[0] /= 4;
    centralPoint[1] /= 4;

    if (!resultLoc.Contains(&centralPoint)) return false;
    if (isWithin10Percent(location.GetArea(), resultLoc.GetArea())) return true;
    return false;
}

std::tuple<std::vector<CQuadrilateral>, std::vector<CQuadrilateral>> removeIfLocExistsInResultLocs(
    const std::vector<CQuadrilateral>& locations,
    const std::vector<CQuadrilateral>& resultLocs)
{
    if (locations.empty() || resultLocs.empty()) {
        return std::make_tuple(locations, resultLocs);
    }

    std::vector<CQuadrilateral> retLoc, retResLoc;
    std::set<int> excludedLoc, includeResLocs;

    for (size_t i = 0; i < locations.size(); ++i) {
        for (size_t j = 0; j < resultLocs.size(); ++j) {
            if (seemAsSameLocation(locations[i], resultLocs[j])) {
                includeResLocs.insert(j);
                excludedLoc.insert(i);
                break;
            }
        }
    }

    for (int j : includeResLocs) {
        retResLoc.push_back(resultLocs[j]);
    }

    for (size_t i = 0; i < locations.size(); ++i) {
        if (excludedLoc.find(i) == excludedLoc.end()) {
            retLoc.push_back(locations[i]);
        }
    }

    return make_tuple(retLoc, retResLoc);
}

CImageData* drawOnImage(const CImageData* image, const std::vector<CQuadrilateral>& locations, const std::vector<CQuadrilateral>& resultLocs)
{
    CImageData* ret = nullptr;
    CImageDrawer drawer;
    auto tupleLoc = removeIfLocExistsInResultLocs(locations, resultLocs);
    if (!std::get<0>(tupleLoc).empty())
        ret = drawer.DrawOnImage(image, std::get<0>(tupleLoc).data(), std::get<0>(tupleLoc).size(), 0xFFFF0000, 2);
    if (!std::get<1>(tupleLoc).empty())
    {
        CImageData* temp = nullptr;
        temp = drawer.DrawOnImage(ret ? ret : image, std::get<1>(tupleLoc).data(), std::get<1>(tupleLoc).size(), 0xFF00FF00, 2);
        if (ret)
            delete ret, ret = nullptr;
        ret = temp;
    }
    return ret;
}
int main() {
    int errorCode;
    char errorMessage[1024];

    // Initialize license.
    // The string 'DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9' here is a free public trial license. Note that network connection is required for this license to work.
    // You can also request a 30-day trial license in the customer portal: https://www.dynamsoft.com/customer/license/trialLicense?product=dbr&utm_source=samples&package=c_cpp
    errorCode = CLicenseManager::InitLicense("DLS2eyJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSJ9", errorMessage, 1024);
    if (errorCode != ErrorCode::EC_OK && errorCode != ErrorCode::EC_LICENSE_WARNING) {
        std::cout << "License initialization failed. ErrorCode: " << errorCode << std::endl;
        return -1;
    }

    CCaptureVisionRouter* cvRouter = new CCaptureVisionRouter();

    MyIntermediateResultReceiver* irr = new MyIntermediateResultReceiver();
    CIntermediateResultManager* irm = cvRouter->GetIntermediateResultManager();
    irm->AddResultReceiver(irr);

    std::string imagePath = "../../Images/GeneralBarcodes.png";
    std::string resultImagePath = "../../Images/GeneralBarcodes_result.png";

    CCapturedResult* result = cvRouter->Capture(imagePath.c_str(), CPresetTemplate::PT_READ_BARCODES);

    if (result->GetErrorCode() == ErrorCode::EC_UNSUPPORTED_JSON_KEY_WARNING)
    {
        std::cout << "Warning: " << result->GetErrorCode() << ", " << result->GetErrorString() << std::endl;
    }
    else if (result->GetErrorCode() != ErrorCode::EC_OK)
    {
        std::cout << "Error: " << result->GetErrorCode() << ", " << result->GetErrorString() << std::endl;
    }
    CDecodedBarcodesResult* barcodeResult = result->GetDecodedBarcodesResult();

    if (irr->locations.empty() && ((!barcodeResult) || barcodeResult->GetItemsCount() == 0))
    {
        std::cout << "No barcode found." << std::endl;
    }
    else
    {
        std::vector<CQuadrilateral> resultLocs;
        if (barcodeResult)
        {
            int count = barcodeResult->GetItemsCount();
            for (int i = 0; i < count; ++i)
            {
                const CBarcodeResultItem* item = barcodeResult->GetItem(i);
                if (item)
                    resultLocs.emplace_back(item->GetLocation());
            }
        }
        CImageIO io;
        CImageData* image = io.ReadFromFile(imagePath.c_str());
        CImageData* drawedImage = drawOnImage(image, irr->locations, resultLocs);
        if (drawedImage)
        {
            io.SaveToFile(drawedImage, resultImagePath.c_str());
            delete drawedImage, drawedImage = nullptr;
#ifdef _WIN32
            system(("start " + resultImagePath).c_str());
#elif __APPLE__
            system(("open " + resultImagePath).c_str());
#else
            int ret = system(("xdg-open " + resultImagePath).c_str());
			if (ret != 0)
			{
				std::cout << "Failed to open the result image. Please check the file manually: " << resultImagePath << std::endl;
			}
#endif
        }
    }
    if (result)
        result->Release();

    delete irr, irr = nullptr;
    delete cvRouter, cvRouter = nullptr;

    return 0;
}