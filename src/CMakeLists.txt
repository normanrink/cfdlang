
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")

set(CFDLANG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(include)

add_subdirectory(extra)
add_subdirectory(lib)
add_subdirectory(tools)

