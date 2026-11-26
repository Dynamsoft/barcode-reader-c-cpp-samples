cmake_minimum_required(VERSION 3.6)

project(GridBarcodeScanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/GridBarcodeScanner.cpp)
set(TARGET_NAME GridBarcodeScanner)

set(OPENCV_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../thirdLib/opencv)
set(OPENCV_INCLUDE_DIR ${OPENCV_ROOT}/include)

# OpenCV libraries live under lib/<platform>/<arch>; the directory name doubles as the
# architecture declaration, so no binary-format sniffing is required.
set(DS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(DS_INCLUDE_PATH ${DS_ROOT}/Include)
if(APPLE)
	set(DS_LIB_PATH ${DS_ROOT}/Dist/Lib/Mac/universal2)
	set(DS_LIB_SUFFIX "")
	set(OPENCV_LIB_EXT dylib)
	set(OPENCV_LIB_DIR ${OPENCV_ROOT}/lib/Mac/universal2)
	set(SAMPLE_ORIGIN_RPATH "@loader_path")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L${DS_LIB_PATH} -Wl,-rpath,${DS_LIB_PATH} -Wl,-rpath,${OPENCV_LIB_DIR} -Wl,-rpath,${SAMPLE_ORIGIN_RPATH}")
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

	# OpenCV: the platform/architecture directory is authoritative. A missing folder
	# means the matching OpenCV build has not been placed yet.
	string(TOLOWER "${TARGET_ARCH}" _OPENCV_ARCH_DIR_NAME)
	set(OPENCV_LIB_DIR "${OPENCV_ROOT}/lib/Windows/${_OPENCV_ARCH_DIR_NAME}")
	if(NOT EXISTS "${OPENCV_LIB_DIR}/opencv_core452.lib")
		message(FATAL_ERROR "OpenCV libraries for ${TARGET_ARCH} not found at ${OPENCV_LIB_DIR} (expected opencv_core452.lib and friends). Place the ${TARGET_ARCH} OpenCV build there.")
	endif()

	# No rpath on Windows: DLLs load from the executable's folder, so the post-build
	# step copies the SDK and OpenCV DLLs next to the executable.
	set(OPENCV_LIB_EXT lib)
	set(SAMPLE_ORIGIN_RPATH "")
	file(GLOB DS_RUNTIME_DLLS "${DS_LIB_PATH}/*.dll")
	file(GLOB OPENCV_RUNTIME_DLLS "${OPENCV_LIB_DIR}/*.dll")
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

	# OpenCV: the platform/architecture directory is authoritative. A missing folder
	# means the matching OpenCV build has not been placed yet.
	if(TARGET_ARCH STREQUAL "ARM64")
		set(OPENCV_LIB_DIR "${OPENCV_ROOT}/lib/Linux/ARM64")
	else()
		set(OPENCV_LIB_DIR "${OPENCV_ROOT}/lib/Linux/x64")
	endif()
	if(NOT EXISTS "${OPENCV_LIB_DIR}/libopencv_core.so")
		message(FATAL_ERROR "OpenCV libraries for ${TARGET_ARCH} not found at ${OPENCV_LIB_DIR} (expected libopencv_core.so and friends). Place the ${TARGET_ARCH} OpenCV build there.")
	endif()

	set(OPENCV_LIB_EXT so)
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
	${DS_LIB_PREFIX}DynamsoftLicense${DS_LIB_SUFFIX}${DS_LIB_EXTENSION}
	${DS_LIB_PREFIX}DynamsoftImageProcessing${DS_LIB_SUFFIX}${DS_LIB_EXTENSION})

if(APPLE)
	set(OPENCV_LIBS
		${OPENCV_LIB_DIR}/libopencv_core.${OPENCV_LIB_EXT}
		${OPENCV_LIB_DIR}/libopencv_highgui.${OPENCV_LIB_EXT}
		${OPENCV_LIB_DIR}/libopencv_imgcodecs.${OPENCV_LIB_EXT}
		${OPENCV_LIB_DIR}/libopencv_imgproc.${OPENCV_LIB_EXT}
	)
elseif(WIN32)
	# The Windows build of OpenCV names its import libraries opencv_<module>452.lib.
	set(OPENCV_LIBS
		${OPENCV_LIB_DIR}/opencv_core452.${OPENCV_LIB_EXT}
		${OPENCV_LIB_DIR}/opencv_highgui452.${OPENCV_LIB_EXT}
		${OPENCV_LIB_DIR}/opencv_imgcodecs452.${OPENCV_LIB_EXT}
		${OPENCV_LIB_DIR}/opencv_imgproc452.${OPENCV_LIB_EXT}
	)
else()
	set(OPENCV_LIBS
		${OPENCV_LIB_DIR}/libopencv_core.${OPENCV_LIB_EXT}
		${OPENCV_LIB_DIR}/libopencv_highgui.${OPENCV_LIB_EXT}
		${OPENCV_LIB_DIR}/libopencv_imgcodecs.${OPENCV_LIB_EXT}
		${OPENCV_LIB_DIR}/libopencv_imgproc.${OPENCV_LIB_EXT}
	)
endif()
if(APPLE OR WIN32)
	set(OPENCV_SYSTEM_LIBS "")
else()
	set(OPENCV_SYSTEM_LIBS pthread dl m z)
endif()

set(DS_MODELS_PATH ${DS_ROOT}/Dist/Models)
set(CUSTOM_TEMPLATE_SOURCE_DIR ${DS_ROOT}/CustomTemplates)
set(CUSTOM_TEMPLATE_FILES "")
foreach(_template GridFastScan.json GridDeepDecode.json)
	set(_template_path "${CUSTOM_TEMPLATE_SOURCE_DIR}/${_template}")
	if(EXISTS "${_template_path}")
		list(APPEND CUSTOM_TEMPLATE_FILES "${_template_path}")
	else()
		message(STATUS "GridBarcodeScanner: ${_template_path} not found, skipping copy")
	endif()
endforeach()

add_executable(${TARGET_NAME} ${SOURCE_FILE})

target_include_directories(${TARGET_NAME} PRIVATE ${DS_INCLUDE_PATH} ${OPENCV_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME} ${DS_LIBS} ${OPENCV_LIBS} ${OPENCV_SYSTEM_LIBS})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
	target_link_libraries(${TARGET_NAME} stdc++fs)
endif()

target_link_directories(${TARGET_NAME} PRIVATE ${DS_LIB_PATH})

set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "${DS_LIB_PATH};${OPENCV_LIB_DIR};${SAMPLE_ORIGIN_RPATH}")

if(CUSTOM_TEMPLATE_FILES)
	add_custom_command(
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CUSTOM_TEMPLATE_FILES} "$<TARGET_FILE_DIR:${TARGET_NAME}>"
		COMMENT "Copying grid scan templates next to the executable"
		VERBATIM
	)
endif()

if(EXISTS "${DS_MODELS_PATH}")
	add_custom_command(
		TARGET ${TARGET_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${DS_MODELS_PATH}" "${DS_LIB_PATH}/Models"
		COMMENT "Copying SDK models"
		VERBATIM
	)
else()
	message(STATUS "GridBarcodeScanner: ${DS_MODELS_PATH} not found, skipping model copy")
endif()

# Windows: place the runtime DLLs and assets next to the executable.
if(WIN32)
	if(DS_RUNTIME_DLLS OR OPENCV_RUNTIME_DLLS)
		add_custom_command(
			TARGET ${TARGET_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DS_RUNTIME_DLLS} ${OPENCV_RUNTIME_DLLS} "$<TARGET_FILE_DIR:${TARGET_NAME}>"
			COMMENT "Copying SDK and OpenCV DLLs next to the executable"
			VERBATIM
		)
	endif()
	if(EXISTS "${DS_ROOT}/Dist/Templates")
		add_custom_command(
			TARGET ${TARGET_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${DS_ROOT}/Dist/Templates" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Templates"
			COMMENT "Copying templates next to the executable"
			VERBATIM
		)
	endif()
	if(EXISTS "${DS_ROOT}/Dist/Models")
		add_custom_command(
			TARGET ${TARGET_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${DS_ROOT}/Dist/Models" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Models"
			COMMENT "Copying models next to the executable"
			VERBATIM
		)
	endif()
endif()

add_custom_target(my_clean
	COMMAND ${CMAKE_COMMAND} -E remove -f ${TARGET_NAME} GridBarcodeScanner.o
)
