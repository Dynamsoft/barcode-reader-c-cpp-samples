cmake_minimum_required(VERSION 3.6)

project(GridBarcodeScanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L ${DBRLIB} -Wl,-rpath,${DBRLIB} -Wl,-rpath,'$ORIGIN' -static-libgcc -static-libstdc++ -s")

set(SOURCE_FILE GridBarcodeScanner.cpp)
set(TARGET_NAME GridBarcodeScanner)

set(OPENCV_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../thirdLib/opencv)
set(OPENCV_INCLUDE_DIR ${OPENCV_ROOT}/include)
set(OPENCV_LIB_DIR ${OPENCV_ROOT}/lib)

set(DS_LIB_PATH ../../Dist/Lib/Linux/x64)
set(DS_LIBS DynamsoftCaptureVisionRouter DynamsoftCore DynamsoftUtility DynamsoftLicense DynamsoftImageProcessing)
set(OPENCV_LIBS
	${OPENCV_LIB_DIR}/libopencv_core.so
	${OPENCV_LIB_DIR}/libopencv_highgui.so
	${OPENCV_LIB_DIR}/libopencv_imgcodecs.so
	${OPENCV_LIB_DIR}/libopencv_imgproc.so
)
set(OPENCV_SYSTEM_LIBS pthread dl m z)

set(DS_MODELS_PATH ../../Dist/Models)
set(CUSTOM_TEMPLATE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../CustomTemplates)
set(CUSTOM_TEMPLATE_FILES
	${CUSTOM_TEMPLATE_SOURCE_DIR}/GridFastScan.json
	${CUSTOM_TEMPLATE_SOURCE_DIR}/GridDeepDecode.json
)

add_executable(${TARGET_NAME} ${SOURCE_FILE})

target_include_directories(${TARGET_NAME} PRIVATE ../../Include ${OPENCV_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME} stdc++ ${DS_LIBS} ${OPENCV_LIBS} ${OPENCV_SYSTEM_LIBS})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
	target_link_libraries(${TARGET_NAME} stdc++fs)
endif()

target_link_directories(${TARGET_NAME} PRIVATE ${DS_LIB_PATH})

set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "${DS_LIB_PATH};./")

add_custom_command(
	TARGET ${TARGET_NAME}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CUSTOM_TEMPLATE_FILES} $<TARGET_FILE_DIR:${TARGET_NAME}>
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${DS_MODELS_PATH} ${DS_LIB_PATH}/Models
)

add_custom_target(my_clean
	COMMAND ${CMAKE_COMMAND} -E remove -f ${TARGET_NAME} GridBarcodeScanner.o
)
