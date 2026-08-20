cmake_minimum_required(VERSION 3.6)
project(BarcodePerformanceEvaluator)

# Absolute path to the SDK distribution folder, independent of the build directory.
get_filename_component(DBR_DIST "${CMAKE_CURRENT_SOURCE_DIR}/../../Dist" ABSOLUTE)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
    # Auto-detect target arch by default; allow manual override.
    set(TARGET_ARCH "AUTO" CACHE STRING "AUTO|X64|ARM32|ARM64")

    if(TARGET_ARCH STREQUAL "AUTO")
        string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYS_PROC)
        if(SYS_PROC MATCHES "aarch64|arm64")
            set(TARGET_ARCH "ARM64")
        elseif(SYS_PROC MATCHES "arm")
            set(TARGET_ARCH "ARM32")
        else()
            set(TARGET_ARCH "X64")
        endif()
    endif()

    if(TARGET_ARCH STREQUAL "X64")
        set(DBRLIB ${DBR_DIST}/Lib/Linux/x64)
    elseif(TARGET_ARCH STREQUAL "ARM32")
        set(DBRLIB ${DBR_DIST}/Lib/Linux/ARM32)
        add_compile_options(-DDM_ARM -D__ARM_NEON__ -mfpu=neon)
    elseif(TARGET_ARCH STREQUAL "ARM64")
        set(DBRLIB ${DBR_DIST}/Lib/Linux/ARM64)
        add_compile_options(-DDM_ARM -D__ARM_NEON__)
    else()
        message(FATAL_ERROR "Unsupported TARGET_ARCH: ${TARGET_ARCH}")
    endif()

    set (CMAKE_CXX_STANDARD 11)
    add_compile_options(-O2 -fPIC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L ${DBRLIB} -static-libgcc -static-libstdc++ -s")
endif()

file(GLOB FILE_SRCS
BarcodePerformanceEvaluator.cpp
BarcodeStatisticsRecorder.cpp
BarcodeFileReader.cpp
DbrBarcodeFileReader.cpp
)

add_executable(BarcodePerformanceEvaluator ${FILE_SRCS})
target_link_libraries(BarcodePerformanceEvaluator DynamsoftCaptureVisionRouter DynamsoftLicense DynamsoftUtility DynamsoftCore)

if(DEFINED DBRLIB)
    set_target_properties(BarcodePerformanceEvaluator PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "${DBRLIB};$ORIGIN")
endif()

# Copy runtime assets next to the executable at build time, never into the SDK folder.
add_custom_command(TARGET BarcodePerformanceEvaluator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:BarcodePerformanceEvaluator>/Templates"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DBR_DIST}/Templates/DBR-PresetTemplates.json"
            "$<TARGET_FILE_DIR:BarcodePerformanceEvaluator>/Templates/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${DBR_DIST}/Models"
            "$<TARGET_FILE_DIR:BarcodePerformanceEvaluator>/Models"
    COMMENT "Copying DBR templates and models next to the executable")