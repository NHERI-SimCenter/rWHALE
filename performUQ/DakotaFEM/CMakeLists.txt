cmake_minimum_required (VERSION 3.1.2)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

project (DakotaFEM VERSION 0.0.1)

include_directories(".")
include_directories("${CMAKE_SOURCE_DIR}/createLOSS/FEMAP58_LU")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/)


add_executable(postprocessDAKOTA postprocessDakota.cpp)
target_link_libraries(postprocessDAKOTA CONAN_PKG::jansson)
set_property(TARGET postprocessDAKOTA PROPERTY CXX_STANDARD 11)

add_executable(extractEDP extractEDP.cpp)
target_link_libraries(extractEDP CONAN_PKG::jansson)
set_property(TARGET extractEDP PROPERTY CXX_STANDARD 11)

file(COPY preprocessJSON.py DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY DakotaFEM.py DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY dpreproSimCenter DESTINATION ${CMAKE_BINARY_DIR}/bin)
