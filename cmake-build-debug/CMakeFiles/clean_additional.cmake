# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/PythonCompilerGUI_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/PythonCompilerGUI_autogen.dir/ParseCache.txt"
  "PythonCompilerGUI_autogen"
  )
endif()
