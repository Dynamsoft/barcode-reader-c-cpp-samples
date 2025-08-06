cmake_minimum_required(VERSION 3.6)

project(ParameterTuner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L ${DBRLIB} -Wl,-rpath,${DBRLIB} -Wl,-rpath,'$ORIGIN' -static-libgcc -static-libstdc++ -s")

set(SOURCE_FILE ParameterTuner.cpp)
set(TARGET_NAME ParameterTuner)

set(DS_LIB_PATH ../../Dist/Lib/Linux/x64)
set(DS_LIBS DynamsoftCaptureVisionRouter DynamsoftCore DynamsoftUtility DynamsoftLicense)

set(DS_JSON_PATH ../../Dist/Templates)
set(DS_MODELS_PATH ../../Dist/Models)

add_executable(${TARGET_NAME} ${SOURCE_FILE})

target_link_libraries(${TARGET_NAME} stdc++ ${DS_LIBS})

target_link_directories(${TARGET_NAME} PRIVATE ${DS_LIB_PATH})

set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "${DS_LIB_PATH};./")

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

add_custom_target(my_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${TARGET_NAME} ParameterTuner.o
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DS_LIB_PATH}/Templates
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DS_LIB_PATH}/Models
)