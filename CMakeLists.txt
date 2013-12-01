cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_INSTALL_PREFIX /opt)
project(proceduralis)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(SFML 2 COMPONENTS graphics audio window system)
include_directories(${SFML_INCLUDE_DIR})

add_definitions(-std=c++0x -g -fpermissive -o0 2>&1)

FILE(GLOB cpp_files "source/*.cpp")
FILE(GLOB hpp_files "source/*.hpp")

add_executable(septillion ${cpp_files} ${hpp_files})
target_link_libraries(septillion ${SFML_LIBRARIES})

install(TARGETS proceduralis DESTINATION proceduralis)
install(DIRECTORY data DESTINATION proceduralis)
install(FILES config.txt DESTINATION proceduralis)

