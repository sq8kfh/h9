cmake_minimum_required(VERSION 3.8)
project(h9)

INCLUDE(FindPkgConfig)

set(CMAKE_C_STANDARD 11)

set(H9D_CONFIG_FILE "./h9d.cfg")
set(H9D_VERSION "0.0.0")

find_package(LibXml2 REQUIRED)

pkg_check_modules(confuse libconfuse)
pkg_check_modules(libxml2 libxml-2.0)

configure_file(config.h.in config.h)

include_directories(/usr/local/include/ ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${LIBXML2_INCLUDE_DIR})
link_directories(/usr/local/lib/)

add_custom_command (
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        COMMAND xxd -i h9msg.xsd ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        DEPENDS h9msg.xsd
)

set(H9_LOG_FILES h9_log.c h9_log.h)
set(H9_MSG_FILES h9_xmlmsg.c h9_xmlmsg.h h9msg.xsd.h h9msg.c)
set(H9_XMLSOCKET_FILES h9_xmlsocket.c h9_xmlsocket.h)

set(H9D_FILES h9d.c h9d_cfg.c h9d_cfg.h h9d_select_event.c h9d_select_event.h h9d_server.c h9d_server.h h9d_client.c h9d_client.h h9d_endpoint.c h9d_endpoint.h h9_slcan.c h9_slcan.h h9d_trigger.c h9d_trigger.h h9d_metricses.c h9d_metricses.h)
set(H9SEND_FILES h9send.c)
set(H9SPY_FILES h9spy.c)
set(H9FUPGRADE_FILES h9fupgrade.c)

add_executable(h9d ${H9D_FILES} ${H9_LOG_FILES} ${H9_MSG_FILES} ${H9_XMLSOCKET_FILES})
TARGET_LINK_LIBRARIES(h9d ${confuse_LIBRARIES} ${libxml2_LIBRARIES})

add_executable(h9send ${H9SEND_FILES} ${H9_LOG_FILES} ${H9_MSG_FILES} ${H9_XMLSOCKET_FILES})
TARGET_LINK_LIBRARIES(h9send ${libxml2_LIBRARIES})

add_executable(h9spy ${H9SPY_FILES} ${H9_LOG_FILES} ${H9_MSG_FILES} ${H9_XMLSOCKET_FILES})
TARGET_LINK_LIBRARIES(h9spy ${libxml2_LIBRARIES})

add_executable(h9fupgrade ${H9FUPGRADE_FILES} ${H9_LOG_FILES} ${H9_MSG_FILES} ${H9_XMLSOCKET_FILES})
TARGET_LINK_LIBRARIES(h9fupgrade ${libxml2_LIBRARIES})
