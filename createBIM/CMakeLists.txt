cmake_minimum_required (VERSION 3.1.2)

add_subdirectory(GenericBimDatabase)
add_subdirectory(RandomUrbanSimDatabase)
add_subdirectory(UrbanSimDatabase)
#add_subdirectory(MongoBimDatabase)

file(COPY common/createBIMCfg.json DESTINATION ${CMAKE_BINARY_DIR}/data)
