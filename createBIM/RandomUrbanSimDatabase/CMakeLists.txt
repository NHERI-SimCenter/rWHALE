cmake_minimum_required (VERSION 3.1.2)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

project (RandomUrbanSimDatabase VERSION 0.0.1)

include_directories(".")
include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories("../UrbanSimDatabase")

file(GLOB SOURCES "RandomUrbanSimDatabase.cpp" "${CMAKE_SOURCE_DIR}/common/*.c" "${CMAKE_SOURCE_DIR}/common/*.cpp")
        
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
add_executable(RandomUrbanSimDatabase ${SOURCES})
target_link_libraries(RandomUrbanSimDatabase CONAN_PKG::jansson)
