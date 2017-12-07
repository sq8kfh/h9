cmake_minimum_required(VERSION 3.8)
project(h9)

INCLUDE(FindPkgConfig)

set(CMAKE_C_STANDARD 11)

set(H9D_CONFIG_FILE "./h9d.cfg")
set(H9D_VERSION "0.0.0")

set(warnings "-Wall -Wextra -Werror -Wdeprecated-declarations")

pkg_check_modules(confuse libconfuse)

configure_file(config.h.in config.h)

set(CMAKE_CXX_FLAGS "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
set(CMAKE_C_FLAGS   "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)

set(SOURCE_FILES h9d.c h9_log.c h9_log.h h9d_cfg.c h9d_cfg.h h9d_select_event.c h9d_select_event.h h9d_server_module.c h9d_server_module.h h9d_client_module.c h9d_client_module.h)

add_executable(h9d ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(h9d "${confuse_LIBRARIES}")
add_executable(h9send h9send.c)
add_executable(h9spy h9spy.c)
