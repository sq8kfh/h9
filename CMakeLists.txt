cmake_minimum_required(VERSION 3.8)
project(h9)

INCLUDE(FindPkgConfig)

set(CMAKE_C_STANDARD 11)

set(H9D_CONFIG_FILE "./h9d.cfg")
set(H9D_VERSION "0.0.0")

find_package(LibXml2 REQUIRED)

pkg_check_modules(confuse libconfuse)
#pkg_check_modules(libxml2 libxml-2.0)

configure_file(config.h.in config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${LIBXML2_INCLUDE_DIR})


add_custom_command (
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        COMMAND xxd -i h9msg.xsd ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        DEPENDS h9msg.xsd
)

set(H9_MSG h9_xmlmsg.c h9_xmlmsg.h h9msg.xsd.h)

set(SOURCE_FILES h9d.c h9_log.c h9_log.h h9d_cfg.c h9d_cfg.h h9d_select_event.c h9d_select_event.h h9d_server_module.c h9d_server_module.h h9d_client_module.c h9d_client_module.h ${H9_MSG})

add_executable(h9d ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(h9d ${confuse_LIBRARIES} ${libxml2_LIBRARIES})
add_executable(h9send h9send.c)
TARGET_LINK_LIBRARIES(h9send ${libxml2_LIBRARIES})
add_executable(h9spy h9spy.c)
