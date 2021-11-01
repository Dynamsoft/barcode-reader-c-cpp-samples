cmake_minimum_required(VERSION 3.6)

project(BatchDecode)

file(GLOB FILE_SRCS
BatchDecode.cpp
BarcodeStatisticsRecorder.cpp
BarcodeFileReader.cpp
DbrBarcodeFileReader.cpp
)

add_executable(BatchDecode ${FILE_SRCS})
set_target_properties(BatchDecode PROPERTIES SKIP_BUILD_RPATH TRUE)
target_link_libraries(BatchDecode DynamsoftBarcodeReader)

