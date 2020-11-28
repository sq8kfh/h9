cmake_minimum_required(VERSION 3.7)

project(h9 VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 14)

set(H9_USER_CONFIG_FILE "~/.h9")
set(H9_CONFIG_PATH "../conf/")
set(H9_VERSION ${PROJECT_VERSION})

find_package(PkgConfig REQUIRED)
find_package(FLEX 2.6.1 REQUIRED)
find_package(BISON 3.3.2 REQUIRED)

pkg_check_modules(CONFUSE libconfuse REQUIRED)
pkg_check_modules(LIBXML2 libxml-2.0 REQUIRED)
pkg_check_modules(READLINE readline>=8.0)

find_program(XXD_EXECUTABLE xxd)

configure_file(config.h.in config.h)

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/ ${CMAKE_SOURCE_DIR})

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
add_subdirectory(h9sniffer)
add_subdirectory(h9stat)
