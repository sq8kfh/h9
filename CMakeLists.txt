cmake_minimum_required(VERSION 3.7)

project(h9 VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 14)

set(H9_USER_CONFIG_FILE "~/.h9")
set(H9D_CONFIG_FILE "./h9d.cfg")
set(H9_BUS_CONFIG_FILE "./h9.cfg")
set(H9_VERSION ${PROJECT_VERSION})

find_package(PkgConfig REQUIRED)
find_package(Flex 2.6.4 REQUIRED)
find_package(Bison 3.4.1 REQUIRED)

pkg_check_modules(CONFUSE libconfuse)
pkg_check_modules(LIBXML2 libxml-2.0)
pkg_check_modules(READLINE readline>=8.0)

find_program(XXD_EXECUTABLE xxd)

configure_file(config.h.in config.h)

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/ ${CMAKE_SOURCE_DIR})
#include_directories(/usr/local/include/ ${CMAKE_SOURCE_DIR}/thirdparty/ ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${LIBXML2_INCLUDE_DIR})
link_directories(/usr/local/lib/ /opt/local/lib/)

add_subdirectory(bus)
add_subdirectory(protocol)
add_subdirectory(drivers)
add_subdirectory(common)
add_subdirectory(h9bus)
add_subdirectory(h9cli)
add_subdirectory(h9d)
add_subdirectory(h9fwu)
add_subdirectory(h9send)
add_subdirectory(h9spy)
add_subdirectory(h9stat)
