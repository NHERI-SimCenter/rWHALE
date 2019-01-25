cmake_minimum_required (VERSION 3.1.2) 
set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

project (GenericBimDatabase VERSION 0.0.1)

include_directories(".")
include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories("../common")

file(GLOB SOURCES "*.cpp" "../common/*.cpp" "${CMAKE_SOURCE_DIR}/common/*.c" "${CMAKE_SOURCE_DIR}/common/*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/)

add_executable(GenericBimDatabase ${SOURCES})
target_link_libraries(GenericBimDatabase CONAN_PKG::jansson)