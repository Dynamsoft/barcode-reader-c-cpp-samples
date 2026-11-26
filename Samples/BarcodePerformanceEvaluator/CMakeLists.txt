cmake_minimum_required(VERSION 3.6)
project(BarcodePerformanceEvaluator)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Absolute path to the SDK distribution folder, independent of the build directory.
get_filename_component(DBR_DIST "${CMAKE_CURRENT_SOURCE_DIR}/../../Dist" ABSOLUTE)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
    if(APPLE)
        set(DBRLIB ${DBR_DIST}/Lib/Mac/universal2)
        set(DS_LIB_SUFFIX "")
        set(SAMPLE_ORIGIN_RPATH "@loader_path")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L ${DBRLIB}")
    elseif(WIN32)
    # CMake links the SDK by explicit full path. Skip the #pragma comment(lib, ...) entries in
    # the sources: their relative paths only resolve when the linker's working directory matches
    # the vcxproj layout, which is not the case for CMake build directories.
    add_compile_definitions(DBR_NO_PRAGMA_LINK)
        # Windows SDK import libraries and DLLs carry an architecture suffix (...x64 / ...x86).
        # AUTO follows the build bitness (CMAKE_SIZEOF_VOID_P); pass -DTARGET_ARCH=X64|X86 to override.
        set(TARGET_ARCH "AUTO" CACHE STRING "Target CPU architecture of the SDK libraries: AUTO|X64|X86")
        if(TARGET_ARCH STREQUAL "AUTO")
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(TARGET_ARCH "X64")
            else()
                set(TARGET_ARCH "X86")
            endif()
        endif()
        if(TARGET_ARCH STREQUAL "X64")
            set(DBRLIB ${DBR_DIST}/Lib/Windows/x64)
            set(DS_LIB_SUFFIX "x64")
        elseif(TARGET_ARCH STREQUAL "X86")
            set(DBRLIB ${DBR_DIST}/Lib/Windows/x86)
            set(DS_LIB_SUFFIX "x86")
        else()
            message(FATAL_ERROR "Unsupported TARGET_ARCH: ${TARGET_ARCH} (expected AUTO, X64 or X86)")
        endif()
        if(NOT EXISTS "${DBRLIB}")
            message(FATAL_ERROR "Dynamsoft SDK libraries for ${TARGET_ARCH} not found at ${DBRLIB}")
        endif()

        # No rpath on Windows: the DLLs are loaded from the executable's folder, so the
        # post-build step copies them next to the executable.
        set(SAMPLE_ORIGIN_RPATH "")
        file(GLOB DS_RUNTIME_DLLS "${DBRLIB}/*.dll")
    else()
        # Auto-detect target arch by default; allow manual override.
        set(TARGET_ARCH "AUTO" CACHE STRING "Target CPU architecture of the SDK libraries: AUTO|X64|ARM64")

        if(TARGET_ARCH STREQUAL "AUTO")
            string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYS_PROC)
            if(SYS_PROC MATCHES "aarch64|arm64")
                set(TARGET_ARCH "ARM64")
            elseif(SYS_PROC MATCHES "arm")
                message(FATAL_ERROR "32-bit ARM (${CMAKE_SYSTEM_PROCESSOR}) is not supported: the SDK only ships x64 and ARM64 libraries.")
            else()
                set(TARGET_ARCH "X64")
            endif()
        endif()

        if(TARGET_ARCH STREQUAL "X64")
            set(DBRLIB ${DBR_DIST}/Lib/Linux/x64)
        elseif(TARGET_ARCH STREQUAL "ARM64")
            set(DBRLIB ${DBR_DIST}/Lib/Linux/ARM64)
            add_compile_options(-DDM_ARM -D__ARM_NEON__)
        else()
            message(FATAL_ERROR "Unsupported TARGET_ARCH: ${TARGET_ARCH} (expected AUTO, X64 or ARM64)")
        endif()

        if(NOT EXISTS "${DBRLIB}")
            message(FATAL_ERROR "Dynamsoft SDK libraries for ${TARGET_ARCH} not found at ${DBRLIB}")
        endif()

        set(SAMPLE_ORIGIN_RPATH "$ORIGIN")
        set(DS_LIB_SUFFIX "")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L ${DBRLIB} -static-libgcc -static-libstdc++ -s")
    endif()

    if(NOT MSVC)
        add_compile_options(-O2 -fPIC)
    endif()
endif()

# Link libraries: on Windows the import libraries are addressed by full path
# (...x64.lib / ...x86.lib); elsewhere plain names resolved through the link directory.
if(WIN32 AND DEFINED DBRLIB)
    set(DS_LIB_PREFIX "${DBRLIB}/")
    set(DS_LIB_EXTENSION ".lib")
else()
    set(DS_LIB_PREFIX "")
    set(DS_LIB_EXTENSION "")
endif()
set(DS_LIBS
    ${DS_LIB_PREFIX}DynamsoftCaptureVisionRouter${DS_LIB_SUFFIX}${DS_LIB_EXTENSION}
    ${DS_LIB_PREFIX}DynamsoftLicense${DS_LIB_SUFFIX}${DS_LIB_EXTENSION}
    ${DS_LIB_PREFIX}DynamsoftUtility${DS_LIB_SUFFIX}${DS_LIB_EXTENSION}
    ${DS_LIB_PREFIX}DynamsoftCore${DS_LIB_SUFFIX}${DS_LIB_EXTENSION})

file(GLOB FILE_SRCS
BarcodePerformanceEvaluator.cpp
BarcodeStatisticsRecorder.cpp
BarcodeFileReader.cpp
DbrBarcodeFileReader.cpp
)

add_executable(BarcodePerformanceEvaluator ${FILE_SRCS})
target_link_libraries(BarcodePerformanceEvaluator ${DS_LIBS})

if(DEFINED DBRLIB)
    set_target_properties(BarcodePerformanceEvaluator PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${DBRLIB};${SAMPLE_ORIGIN_RPATH}")
endif()

# macOS/Linux: the SDK loads templates/models relative to the library location,
# so the assets go into the SDK lib directory, same as the other samples.
if(DEFINED DBRLIB AND NOT WIN32)
    file(COPY ${DBR_DIST}/Templates/DBR-PresetTemplates.json
         DESTINATION ${DBRLIB}/Templates)
    file(COPY ${DBR_DIST}/Models/
         DESTINATION ${DBRLIB}/Models)
endif()

# Windows: templates/models and the SDK DLLs have to sit next to the executable.
if(WIN32)
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

    if(DS_RUNTIME_DLLS)
        add_custom_command(TARGET BarcodePerformanceEvaluator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DS_RUNTIME_DLLS} "$<TARGET_FILE_DIR:BarcodePerformanceEvaluator>"
            COMMENT "Copying SDK DLLs next to the executable"
            VERBATIM)
    endif()
endif()