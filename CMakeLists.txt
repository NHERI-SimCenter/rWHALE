cmake_minimum_required (VERSION 3.1.2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin/)

add_subdirectory(createBIM)
add_subdirectory(createEVENT)
add_subdirectory(createSAM)
add_subdirectory(createEDP)
add_subdirectory(performSIMULATION)
add_subdirectory(performUQ)
add_subdirectory(createLOSS)
