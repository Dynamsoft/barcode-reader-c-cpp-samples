cmake_minimum_required(VERSION 3.6)

project(VideoDecoding)

find_package(OpenCV 3.4.5 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdLib/Include)

file(GLOB FILE_SRCS
VideoDecoding.cpp
)
add_executable(VideoDecoding ${FILE_SRCS})
set_target_properties(VideoDecoding PROPERTIES SKIP_BUILD_RPATH TRUE)
target_link_libraries(VideoDecoding DynamsoftBarcodeReader opencv_core opencv_videoio opencv_highgui)

