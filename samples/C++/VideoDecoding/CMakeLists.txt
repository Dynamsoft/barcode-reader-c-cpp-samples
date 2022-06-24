cmake_minimum_required(VERSION 3.6)
project(VideoDecoding)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
    # set the platform
    option(BUILD_LINUX "Build samples for running on Linux x86_64" ON)
    option(BUILD_ARM "Build samples for running on Linux ARM32" OFF)
    option(BUILD_ARM64 "Build samples for running on Linux ARM64" OFF)

    if(BUILD_LINUX)
        set(DBRLIB ../../../lib/Linux/x64)
    elseif(BUILD_ARM)
        set(DBRLIB ../../../lib/Linux/ARM32)
        add_compile_options(-DDM_ARM -D__ARM_NEON__ -mfpu=neon)
    elseif(BUILD_ARM64)
        set(DBRLIB ../../../lib/Linux/ARM64)
        add_compile_options(-DDM_ARM -D__ARM_NEON__)
    else()
        message(FATAL_ERROR "Please specify a supported platform")
    endif()

    set (CMAKE_CXX_STANDARD 11)
    add_compile_options(-O2 -fPIC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L ${DBRLIB} -Wl,-rpath,${DBRLIB} -Wl,-rpath,'$ORIGIN' -static-libgcc -static-libstdc++ -s")
endif()

find_package(OpenCV 3.4.5 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdLib/Include)

file(GLOB FILE_SRCS
VideoDecoding.cpp
)
add_executable(VideoDecoding ${FILE_SRCS})
set_target_properties(VideoDecoding PROPERTIES SKIP_BUILD_RPATH TRUE)
target_link_libraries(VideoDecoding DynamsoftBarcodeReader opencv_core opencv_videoio opencv_highgui)

