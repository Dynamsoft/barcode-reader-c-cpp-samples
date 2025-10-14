cmake_minimum_required(VERSION 3.6)

project(GeneralSettings)

set(DBRLIB ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist/Lib/Linux/x64)

add_compile_options(-O2 -fPIC)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden -L ${DBRLIB} -Wl,-rpath,${DBRLIB} -Wl,-rpath,'$ORIGIN' -static-libgcc -static-libstdc++ -s")

file(GLOB FILE_SRCS
    GeneralSettings.cpp
)

add_executable(GeneralSettings ${FILE_SRCS})

set_target_properties(GeneralSettings PROPERTIES SKIP_BUILD_RPATH TRUE)

target_link_libraries(GeneralSettings DynamsoftCaptureVisionRouter DynamsoftLicense DynamsoftCore)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist/Templates/DBR-PresetTemplates.json
     DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist/Lib/Linux/x64/Templates)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist//Models/
     DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../Dist/Lib/Linux/x64/Models)