cmake_minimum_required (VERSION 3.1.2)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

project (FEMAP58_LU VERSION 0.0.1)

include_directories(".")
include_directories("include")

file(GLOB SOURCES "*.cpp" "include/*.c" "include/*.cpp")

find_package(Threads REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_executable(FEMAP58_LU ${SOURCES})
target_link_libraries(FEMAP58_LU CONAN_PKG::jansson Threads::Threads)
set_property(TARGET FEMAP58_LU PROPERTY CXX_STANDARD 11)
set_property(TARGET FEMAP58_LU PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

file(COPY data/settings.ini DESTINATION ${CMAKE_BINARY_DIR}/data)
file(COPY data/ATCCurves DESTINATION ${CMAKE_BINARY_DIR}/data)
file(COPY data/normative DESTINATION ${CMAKE_BINARY_DIR}/data)

