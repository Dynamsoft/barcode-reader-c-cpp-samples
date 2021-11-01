cmake_minimum_required(VERSION 3.6)

project(HelloWorld_C)

file(GLOB FILE_SRCS
HelloWorld.c
)
add_executable(HelloWorld_C ${FILE_SRCS})
set_target_properties(HelloWorld_C PROPERTIES SKIP_BUILD_RPATH TRUE)
target_link_libraries(HelloWorld_C DynamsoftBarcodeReader)

