cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_INSTALL_PREFIX /opt)
project(proceduralis)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(SFML 2 COMPONENTS graphics audio window system)
include_directories(${SFML_INCLUDE_DIR} ${OPENCL_INCLUDE_DIR})

find_package(OPENCL)

add_definitions(-std=c++0x -g -fpermissive -o0 2>&1)

set(CMAKE_CXX_FLAGS  -DCL_USE_DEPRECATED_OPENCL_1_1_APIS)

FILE(GLOB cpp_files "source/*.cpp")
FILE(GLOB hpp_files "source/*.hpp")

add_executable(proceduralis ${cpp_files} ${hpp_files})
target_link_libraries(proceduralis ${SFML_LIBRARIES} ${OPENCL_LIBRARIES})

FILE(GLOB OPENCL_FILES "${PROJECT_SOURCE_DIR}/*.cl")

install(TARGETS proceduralis DESTINATION proceduralis)
install(DIRECTORY data DESTINATION proceduralis)
install(FILES config.json DESTINATION proceduralis)
install(FILES ${OPENCL_FILES} DESTINATION proceduralis)
install(FILES README.md DESTINATION proceduralis)
install(FILES license.txt DESTINATION proceduralis)
install(FILES collaborators.txt DESTINATION proceduralis)

