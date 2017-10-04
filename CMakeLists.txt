cmake_minimum_required(VERSION 3.5)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

set(CFDLANG_EXTRA_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/extra)
add_custom_target(ExtraOutputDirectory echo "Making directory ${CFDLANG_EXTRA_OUTPUT_DIR}")
add_custom_command(TARGET ExtraOutputDirectory
                   COMMAND mkdir
                   ARGS -p ${CFDLANG_EXTRA_OUTPUT_DIR})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "" FORCE)
endif()

add_subdirectory(src)

include_directories(src/include)
add_subdirectory(tests)
