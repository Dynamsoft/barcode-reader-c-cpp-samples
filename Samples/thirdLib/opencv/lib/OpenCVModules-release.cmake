#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "opencv_core" for configuration "Release"
set_property(TARGET opencv_core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_core PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_core452.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_core452.dll"
  )

list(APPEND _cmake_import_check_targets opencv_core )
list(APPEND _cmake_import_check_files_for_opencv_core "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_core452.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_core452.dll" )

# Import target "opencv_imgproc" for configuration "Release"
set_property(TARGET opencv_imgproc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgproc PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_imgproc452.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_imgproc452.dll"
  )

list(APPEND _cmake_import_check_targets opencv_imgproc )
list(APPEND _cmake_import_check_files_for_opencv_imgproc "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_imgproc452.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_imgproc452.dll" )

# Import target "opencv_imgcodecs" for configuration "Release"
set_property(TARGET opencv_imgcodecs APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgcodecs PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_imgcodecs452.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_imgcodecs452.dll"
  )

list(APPEND _cmake_import_check_targets opencv_imgcodecs )
list(APPEND _cmake_import_check_files_for_opencv_imgcodecs "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_imgcodecs452.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_imgcodecs452.dll" )

# Import target "opencv_videoio" for configuration "Release"
set_property(TARGET opencv_videoio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_videoio PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_videoio452.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_videoio452.dll"
  )

list(APPEND _cmake_import_check_targets opencv_videoio )
list(APPEND _cmake_import_check_files_for_opencv_videoio "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_videoio452.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_videoio452.dll" )

# Import target "opencv_highgui" for configuration "Release"
set_property(TARGET opencv_highgui APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_highgui PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_highgui452.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_highgui452.dll"
  )

list(APPEND _cmake_import_check_targets opencv_highgui )
list(APPEND _cmake_import_check_files_for_opencv_highgui "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_highgui452.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_highgui452.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
