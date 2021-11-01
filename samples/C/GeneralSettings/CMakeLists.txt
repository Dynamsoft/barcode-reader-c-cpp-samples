cmake_minimum_required(VERSION 3.6)

project(GeneralSettings_C)

file(GLOB FILE_SRCS
GeneralSettings.c
)
add_executable(GeneralSettings_C ${FILE_SRCS})
set_target_properties(GeneralSettings_C PROPERTIES SKIP_BUILD_RPATH TRUE)
target_link_libraries(GeneralSettings_C DynamsoftBarcodeReader)

