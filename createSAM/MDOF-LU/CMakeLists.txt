cmake_minimum_required (VERSION 3.1.2)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

project (MDOF-LU VERSION 0.0.1)

include_directories(".")

file(GLOB SOURCES "*.cpp")
        
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/)

add_executable(MDOF-LU ${SOURCES})
target_link_libraries(MDOF-LU CONAN_PKG::jansson)

file(COPY data/HazusData.txt DESTINATION ${CMAKE_BINARY_DIR}/data)
