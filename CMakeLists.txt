cmake_minimum_required(VERSION 3.24)

project(h9 VERSION 0.3.0)

set(CMAKE_CXX_STANDARD 17)

option(H9_DEBUG "Enable debug")

set(H9_USER_CONFIG_FILE "~/.h9")
set(H9_CONFIG_PATH "../conf/")
set(H9_VERSION ${PROJECT_VERSION})

find_package(Python3 3.9 EXACT REQUIRED Interpreter Development)
find_package(PkgConfig REQUIRED)

find_package(fmt 9.0 REQUIRED)
find_package(spdlog 1.10 REQUIRED)
find_package(nlohmann_json 3.11.2 REQUIRED)
#find_package(FLEX 2.6.1 REQUIRED)
#find_package(BISON 3.3.2 REQUIRED)

pkg_check_modules(CONFUSE libconfuse REQUIRED)
pkg_check_modules(READLINE readline>=8.0)

message("-! Python version ${Python3_VERSION}")

if(NOT READLINE_FOUND)
    message(STATUS "Checking for module 'readline>=7.0'")
    include(cmake/FindReadline.cmake)
    if(Readline_VERSION VERSION_GREATER_EQUAL "7.0") 
	    message(STATUS "  Found readline ${Readline_VERSION}")
    else()
	    message(FATAL_ERROR "  No package 'readline' found")
    endif()
    set(READLINE_LIBRARIES ${Readline_LIBRARIES})
    set(READLINE_INCLUDE_DIRS ${Readline_INCLUDE_DIRS})
endif()

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/ ${CMAKE_SOURCE_DIR})

link_directories(/usr/local/lib/ /opt/local/lib/)

add_subdirectory(src)

#enable_testing()
#
#add_test(
#        NAME cpp_test
#        COMMAND false
#)