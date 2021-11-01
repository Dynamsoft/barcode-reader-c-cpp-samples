cmake_minimum_required(VERSION 3.6)

project(HelloWorld)

file(GLOB FILE_SRCS
HelloWorld.cpp
)
add_executable(HelloWorld ${FILE_SRCS})
set_target_properties(HelloWorld PROPERTIES SKIP_BUILD_RPATH TRUE)
target_link_libraries(HelloWorld DynamsoftBarcodeReader)

