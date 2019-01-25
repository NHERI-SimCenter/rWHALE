cmake_minimum_required (VERSION 3.1.2)
set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

project (MultipleFidelity VERSION 0.0.1)

include_directories(".")
include_directories("../MDOF-LU")
include_directories("../ConcreteShearWall")

file(GLOB SOURCES "*.cpp" "../ConcreteShearWall/*.cpp" "../MDOF-LU/*.cpp")

list(FILTER SOURCES EXCLUDE REGEX ".+MDOF_ShearBuildingModel.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".+processSAM.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/)

add_executable(MultipleFidelity ${SOURCES})
target_link_libraries(MultipleFidelity CONAN_PKG::jansson)