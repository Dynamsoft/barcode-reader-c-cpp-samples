cmake_minimum_required(VERSION 3.6)
project(GeneralSettings_C)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
    # set the platform
    option(BUILD_LINUX "Build samples for running on Linux x86_64" ON)
    option(BUILD_ARM "Build samples for running on Linux ARM32" OFF)
    option(BUILD_ARM64 "Build samples for running on Linux ARM64" OFF)

    if(BUILD_LINUX)
        set(DBRLIB ../../../lib/Linux)
    elseif(BUILD_ARM)
        set(DBRLIB ../../../lib/ARM32)
        add_compile_options(-DDM_ARM -D__ARM_NEON__ -mfpu=neon)
    elseif(BUILD_ARM64)
        set(DBRLIB ../../../lib/ARM64)
        add_compile_options(-DDM_ARM -D__ARM_NEON__)
    else()
        message(FATAL_ERROR "Please specify a supported platform")
    endif()

    set (CMAKE_CXX_STANDARD 11)
    add_compile_options(-O2 -fPIC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L ${DBRLIB} -Wl,-rpath,${DBRLIB} -Wl,-rpath,'$ORIGIN' -static-libgcc -static-libstdc++ -s")
endif()


file(GLOB FILE_SRCS
GeneralSettings.c
)
add_executable(GeneralSettings_C ${FILE_SRCS})
set_target_properties(GeneralSettings_C PROPERTIES SKIP_BUILD_RPATH TRUE)
target_link_libraries(GeneralSettings_C DynamsoftBarcodeReader)

