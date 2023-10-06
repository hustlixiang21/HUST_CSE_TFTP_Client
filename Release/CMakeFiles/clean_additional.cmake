# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\socket_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\socket_autogen.dir\\ParseCache.txt"
  "socket_autogen"
  )
endif()
