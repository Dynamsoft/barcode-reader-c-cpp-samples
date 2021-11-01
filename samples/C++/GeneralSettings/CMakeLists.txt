cmake_minimum_required(VERSION 3.6)

project(GeneralSettings)

file(GLOB FILE_SRCS
GeneralSettings.cpp
)
add_executable(GeneralSettings ${FILE_SRCS})
set_target_properties(GeneralSettings PROPERTIES SKIP_BUILD_RPATH TRUE)
target_link_libraries(GeneralSettings DynamsoftBarcodeReader)

