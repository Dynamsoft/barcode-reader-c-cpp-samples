cmake_minimum_required(VERSION 3.6)

project(ShowLocalizedVSDecodedBarcodes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILE ShowLocalizedVSDecodedBarcodes.cpp)
set(TARGET_NAME ShowLocalizedVSDecodedBarcodes)

set(DS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
if(APPLE)
    set(DS_LIB_PATH ${DS_ROOT}/Dist/Lib/Mac/universal2)
    set(DS_LIB_SUFFIX "")
    set(SAMPLE_ORIGIN_RPATH "@loader_path")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L${DS_LIB_PATH} -Wl,-rpath,${DS_LIB_PATH} -Wl,-rpath,${SAMPLE_ORIGIN_RPATH}")
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
        set(DS_LIB_PATH ${DS_ROOT}/Dist/Lib/Windows/x64)
        set(DS_LIB_SUFFIX "x64")
    elseif(TARGET_ARCH STREQUAL "X86")
        set(DS_LIB_PATH ${DS_ROOT}/Dist/Lib/Windows/x86)
        set(DS_LIB_SUFFIX "x86")
    else()
        message(FATAL_ERROR "Unsupported TARGET_ARCH: ${TARGET_ARCH} (expected AUTO, X64 or X86)")
    endif()
    if(NOT EXISTS "${DS_LIB_PATH}")
        message(FATAL_ERROR "Dynamsoft SDK libraries for ${TARGET_ARCH} not found at ${DS_LIB_PATH}")
    endif()

    # No rpath on Windows: the DLLs are loaded from the executable's folder, so the
    # post-build step copies them next to the executable.
    set(SAMPLE_ORIGIN_RPATH "")
    file(GLOB DS_RUNTIME_DLLS "${DS_LIB_PATH}/*.dll")
else()
    # Pick the SDK libraries that match the target CPU architecture. AUTO derives the
    # architecture from the compiler target; pass -DTARGET_ARCH=X64|ARM64 when
    # cross-compiling for an architecture other than the build host's.
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
        set(DS_LIB_PATH ${DS_ROOT}/Dist/Lib/Linux/x64)
    elseif(TARGET_ARCH STREQUAL "ARM64")
        set(DS_LIB_PATH ${DS_ROOT}/Dist/Lib/Linux/ARM64)
        add_compile_options(-DDM_ARM -D__ARM_NEON__)
    else()
        message(FATAL_ERROR "Unsupported TARGET_ARCH: ${TARGET_ARCH} (expected AUTO, X64 or ARM64)")
    endif()
    if(NOT EXISTS "${DS_LIB_PATH}")
        message(FATAL_ERROR "Dynamsoft SDK libraries for ${TARGET_ARCH} not found at ${DS_LIB_PATH}")
    endif()

    set(SAMPLE_ORIGIN_RPATH "$ORIGIN")
    set(DS_LIB_SUFFIX "")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L${DS_LIB_PATH} -Wl,-rpath,${DS_LIB_PATH} -Wl,-rpath,'$ORIGIN' -static-libgcc -static-libstdc++ -s")
endif()

# Link libraries: on Windows the import libraries are addressed by full path
# (...x64.lib / ...x86.lib); elsewhere plain names resolved through the link directory.
if(WIN32)
    set(DS_LIB_PREFIX "${DS_LIB_PATH}/")
    set(DS_LIB_EXTENSION ".lib")
else()
    set(DS_LIB_PREFIX "")
    set(DS_LIB_EXTENSION "")
endif()
set(DS_LIBS
    ${DS_LIB_PREFIX}DynamsoftCaptureVisionRouter${DS_LIB_SUFFIX}${DS_LIB_EXTENSION}
    ${DS_LIB_PREFIX}DynamsoftCore${DS_LIB_SUFFIX}${DS_LIB_EXTENSION}
    ${DS_LIB_PREFIX}DynamsoftUtility${DS_LIB_SUFFIX}${DS_LIB_EXTENSION}
    ${DS_LIB_PREFIX}DynamsoftLicense${DS_LIB_SUFFIX}${DS_LIB_EXTENSION})

set(DS_JSON_PATH ${DS_ROOT}/Dist/Templates)
set(DS_MODELS_PATH ${DS_ROOT}/Dist/Models)

add_executable(${TARGET_NAME} ${SOURCE_FILE})

target_link_libraries(${TARGET_NAME} ${DS_LIBS})

target_link_directories(${TARGET_NAME} PRIVATE ${DS_LIB_PATH})

set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "${DS_LIB_PATH};${SAMPLE_ORIGIN_RPATH}")

add_custom_command(
    TARGET ${TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DS_JSON_PATH} ${DS_LIB_PATH}/Templates
)

add_custom_command(
    TARGET ${TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DS_MODELS_PATH} ${DS_LIB_PATH}/Models
)

# Windows: place the runtime DLLs and assets next to the executable.
if(WIN32)
    if(DS_RUNTIME_DLLS)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DS_RUNTIME_DLLS} "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            COMMENT "Copying SDK DLLs next to the executable"
            VERBATIM)
    endif()
    if(EXISTS "${DS_JSON_PATH}")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${DS_JSON_PATH}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Templates"
            COMMENT "Copying templates next to the executable"
            VERBATIM)
    endif()
    if(EXISTS "${DS_MODELS_PATH}")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${DS_MODELS_PATH}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Models"
            COMMENT "Copying models next to the executable"
            VERBATIM)
    endif()
    if(EXISTS "${DS_ROOT}/Dist/ParserResources")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${DS_ROOT}/Dist/ParserResources" "$<TARGET_FILE_DIR:${TARGET_NAME}>/ParserResources"
            COMMENT "Copying parser resources next to the executable"
            VERBATIM)
    endif()
endif()

add_custom_target(my_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${TARGET_NAME} ShowLocalizedVSDecodedBarcodes.o
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DS_LIB_PATH}/Templates
)