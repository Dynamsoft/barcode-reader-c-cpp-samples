cmake_minimum_required(VERSION 3.6)
project(VideoDecoding)

set(DBRLIB ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist/Lib/Linux/x64)

set (CMAKE_CXX_STANDARD 11)
add_compile_options(-O2 -fPIC)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L ${DBRLIB} -Wl,-rpath,${DBRLIB} -Wl,-rpath,'$ORIGIN' -static-libgcc -static-libstdc++ -s")
find_package(OpenCV 3.4.5 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdLib/Include)

file(GLOB FILE_SRCS
VideoDecoding.cpp
)
add_executable(VideoDecoding ${FILE_SRCS})
set_target_properties(VideoDecoding PROPERTIES SKIP_BUILD_RPATH TRUE)
target_link_libraries(VideoDecoding DynamsoftCaptureVisionRouter DynamsoftLicense DynamsoftUtility DynamsoftCore opencv_core opencv_videoio opencv_highgui)
File(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist/Templates/DBR-PresetTemplates.json DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist/Lib/Linux/x64/Templates)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist/Models DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist/Lib/Linux/x64)