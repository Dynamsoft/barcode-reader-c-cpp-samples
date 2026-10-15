cmake_minimum_required(VERSION 3.6)

project(VideoDecoding LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/VideoDecoding.cpp)
set(TARGET_NAME VideoDecoding)

# Use the OpenCV shipped with this repository instead of the one installed on the machine.
set(OPENCV_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../thirdLib/opencv)
set(OPENCV_INCLUDE_DIR ${OPENCV_ROOT}/include)
set(OPENCV_LIB_DIR ${OPENCV_ROOT}/lib)

set(DS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(DS_INCLUDE_PATH ${DS_ROOT}/Include)
set(DS_LIB_PATH ${DS_ROOT}/Dist/Lib/Linux/x64)

add_compile_options(-O2 -fPIC)
# --disable-new-dtags emits DT_RPATH instead of DT_RUNPATH, so the rpath below also
# applies to transitive dependencies (e.g. libopencv_imgproc.so pulled in by highgui).
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L${DS_LIB_PATH} -Wl,--disable-new-dtags -Wl,-rpath,${DS_LIB_PATH} -Wl,-rpath,${OPENCV_LIB_DIR} -Wl,-rpath,'$ORIGIN' -static-libgcc -static-libstdc++ -s")

set(DS_LIBS DynamsoftCaptureVisionRouter DynamsoftLicense DynamsoftUtility DynamsoftCore)

set(OPENCV_LIBS
	${OPENCV_LIB_DIR}/libopencv_core.so
	${OPENCV_LIB_DIR}/libopencv_videoio.so
	${OPENCV_LIB_DIR}/libopencv_highgui.so
)

set(OPENCV_SYSTEM_LIBS pthread dl m z)

add_executable(${TARGET_NAME} ${SOURCE_FILE})

target_include_directories(${TARGET_NAME} PRIVATE ${DS_INCLUDE_PATH} ${OPENCV_INCLUDE_DIR})

target_link_directories(${TARGET_NAME} PRIVATE ${DS_LIB_PATH})

target_link_libraries(${TARGET_NAME} ${DS_LIBS} ${OPENCV_LIBS} ${OPENCV_SYSTEM_LIBS})

set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "${DS_LIB_PATH};${OPENCV_LIB_DIR};$ORIGIN")

file(COPY ${DS_ROOT}/Dist/Templates/DBR-PresetTemplates.json DESTINATION ${DS_LIB_PATH}/Templates)
file(COPY ${DS_ROOT}/Dist/Models DESTINATION ${DS_LIB_PATH})