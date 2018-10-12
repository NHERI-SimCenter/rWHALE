cmake_minimum_required (VERSION 3.1.2)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

project (LLNL_SW4 VERSION 0.0.1)

include_directories(".")
include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories("${CMAKE_SOURCE_DIR}/include/nanoflann")

file(GLOB SOURCES "LLNL_SW4.cpp" "${CMAKE_SOURCE_DIR}/common/*.c" "${CMAKE_SOURCE_DIR}/common/*.cpp")
        
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/)

add_executable(LLNL_SW4 ${SOURCES})
target_link_libraries(LLNL_SW4 CONAN_PKG::jansson)

file(COPY HFmeta DESTINATION ${CMAKE_BINARY_DIR}/data)
