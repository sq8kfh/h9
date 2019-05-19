cmake_minimum_required(VERSION 3.8)
project(h9)

INCLUDE(FindPkgConfig)

set(CMAKE_CXX_STANDARD 14)

set(H9D_CONFIG_FILE "./h9d.cfg")
set(H9_USER_CONFIG_FILE "~/.h9.cfg")
set(H9_BUS_CONFIG_FILE "./h9.cfg")
set(H9_VERSION "0.2.0")

find_package(LibXml2 REQUIRED)

pkg_check_modules(confuse libconfuse)
pkg_check_modules(libxml2 libxml-2.0)

configure_file(config.h.in config.h)

include_directories(/usr/local/include/ ${CMAKE_SOURCE_DIR}/thirdparty/ ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${LIBXML2_INCLUDE_DIR})
link_directories(/usr/local/lib/)

add_subdirectory(common)
add_subdirectory(h9bus)
add_subdirectory(h9cli)
add_subdirectory(h9d)
add_subdirectory(h9fwu)
add_subdirectory(h9send)
add_subdirectory(h9spy)

#add_custom_command (
#        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
#        COMMAND xxd -i h9msg.xsd ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
#        DEPENDS h9msg.xsd
#)
#
#set(H9_LOG_FILES common/h9_log.c common/h9_log.h)
#set(H9_MSG_FILES h9_xmlmsg.c h9_xmlmsg.h h9msg.xsd.h h9msg.c h9msg.h)
#set(H9_XMLSOCKET_FILES h9_xmlsocket.c h9_xmlsocket.h)
#set(H9_DEVS drivers/slcan.c drivers/slcan.h drivers/h9_dev.c drivers/h9_dev.h drivers/loop.c drivers/loop.h)
#
#set(H9D_FILES h9d.c h9d_cfg.c h9d_cfg.h h9d_select_event.c h9d_select_event.h h9d_server.c h9d_server.h h9d_client.c h9d_client.h h9d_endpoint.c h9d_endpoint.h h9d_trigger.c h9d_trigger.h h9d_metricses.c h9d_metricses.h)
#set(H9FW_FILES h9fw.c)
#set(H9SEND_FILES h9send.c)
#set(H9SPY_FILES h9spy.c)
#set(H9STATS_FILES h9stats.cc)

#add_executable(h9d ${H9D_FILES} ${H9_LOG_FILES} ${H9_MSG_FILES} ${H9_XMLSOCKET_FILES} ${H9_DEVS})
#TARGET_LINK_LIBRARIES(h9d ${confuse_LIBRARIES} ${libxml2_LIBRARIES})

#add_executable(h9fw ${H9FW_FILES} ${H9_LOG_FILES} ${H9_MSG_FILES} ${H9_XMLSOCKET_FILES})
#TARGET_LINK_LIBRARIES(h9fw ${libxml2_LIBRARIES})

#add_executable(h9send ${H9SEND_FILES} ${H9_LOG_FILES} ${H9_MSG_FILES} ${H9_XMLSOCKET_FILES})
#TARGET_LINK_LIBRARIES(h9send ${libxml2_LIBRARIES})

#add_executable(h9spy ${H9SPY_FILES} ${H9_LOG_FILES} ${H9_MSG_FILES} ${H9_XMLSOCKET_FILES})
#TARGET_LINK_LIBRARIES(h9spy ${libxml2_LIBRARIES})

#add_executable(h9stats ${H9STATS_FILES} ${H9_LOG_FILES} ${H9_MSG_FILES} ${H9_XMLSOCKET_FILES})
#TARGET_LINK_LIBRARIES(h9stats ${libxml2_LIBRARIES})
