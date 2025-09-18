cmake_minimum_required(VERSION 3.22)
project(https_server)
set (CMAKE_CXX_STANDARD 11)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

find_package(Boost REQUIRED)

set(EXECUTABLE_NAME Executable)
set(HEADERS_LIB headers_lib)

add_subdirectory(src)
add_subdirectory(app)
