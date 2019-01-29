cmake_minimum_required (VERSION 3.1.2)
set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

project (PerformSimulation VERSION 0.0.1)

include_directories("./common")
include_directories("./OpenSeesSimulation")

file(GLOB PRESOURCES "./OpenSeesSimulation/*Pre*.cpp" "./OpenSeesSimulation/OpenSeesConcreteShearWalls.cpp")
file(GLOB POSTSOURCES "./OpenSeesSimulation/*Post*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/)

add_executable(getUncertainty common/getUncertainty.cpp ${PRESOURCES})
target_link_libraries(getUncertainty CONAN_PKG::jansson)

add_executable(mainPreprocessor common/mainPreprocessor.cpp ${PRESOURCES})
target_link_libraries(mainPreprocessor CONAN_PKG::jansson)

add_executable(mainPostprocessor common/mainPostprocessor.cpp ${POSTSOURCES})
target_link_libraries(mainPostprocessor CONAN_PKG::jansson)

file(COPY OpenSeesSimulation/OpenSeesSimulation.py DESTINATION ${CMAKE_BINARY_DIR}/bin)
