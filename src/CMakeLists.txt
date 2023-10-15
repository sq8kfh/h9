if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    message("-! Using epoll")
    SET(H9D_EVENT epoll.cc epoll.h)

    message("-! Enable SocketCAN driver")
    SET(H9_SOCKETCAN_DRIVER socketcan_driver.cc socketcan_driver.h)
else()
    message("-! Using kqueue")
    SET(H9D_EVENT kqueue.cc kqueue.h)

    message("-! Disable SocketCAN driver")
    SET(H9_SOCKETCAN_DRIVER "")
endif()

configure_file(config.h.in config.h)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

set(H9D_SRC h9d.cc config.h
        h9d_configurator.cc h9d_configurator.h
        h9frame.cc h9frame.h
        ext_h9frame.cc ext_h9frame.h
        busframe.cc busframe.h
        bus_driver.cc bus_driver.h
        loop_driver.cc loop_driver.h
        slcan_driver.cc slcan_driver.h
        ${H9_SOCKETCAN_DRIVER}
        ${H9D_EVENT}
        bus.cc bus.h
        framesubject.cc frameobserver.h
        frameobserver.cc frameobserver.h
        h9framecomparator.cc h9framecomparator.h
        tcpserver.cc tcpserver.h
        tcpclientthread.cc tcpclientthread.h
        h9socket.cc h9socket.h
        h9msgsocket.cc h9msgsocket.h
        api.cc api.h
        client_frame_obs.cc client_frame_obs.h
        metrics_collector.cc metrics_collector.h
        node_mgr.cc node_mgr.h
        node.cc node.h
        send_frame_result.h
        devices_mgr.cc devices_mgr.h
        device.cc device.h
        devicedescloader.cc devicedescloader.h
        virtual_driver.cc virtual_driver.h
        virtual_endpoint.cc virtual_endpoint.h
        virtual_py_node.cc virtual_py_node.h
        py_h9.cc py_h9.h
        dev_node_exception.cc dev_node_exception.h)

set(CLIENT_COMMON_SRC config.h
        h9frame.cc h9frame.h
        ext_h9frame.cc ext_h9frame.h
        h9socket.cc h9socket.h
        h9msgsocket.cc h9msgsocket.h
        h9connector.cc h9connector.h
        h9_configurator.cc h9_configurator.h)

set(H9SNIFFER_SRC h9sniffer.cc config.h)

set(H9SEND_SRC h9send.cc config.h)

set(H9STATS_SRC h9stats.cc config.h)

add_custom_command(OUTPUT git_version.h
                  COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/generate_git_version_header.py > ${PROJECT_SOURCE_DIR}/src/git_version.h~
                  COMMAND /bin/sh -c 'diff ${PROJECT_SOURCE_DIR}/src/git_version.h~ ${PROJECT_SOURCE_DIR}/src/git_version.h 2> /dev/null > /dev/null || { cp ${PROJECT_SOURCE_DIR}/src/git_version.h~ ${PROJECT_SOURCE_DIR}/src/git_version.h\; }' DEPENDS ${H9D_SRC})

add_executable(h9d ${H9D_SRC} git_version.h)
target_include_directories(h9d PUBLIC ${CONFUSE_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS})
target_compile_options(h9d PUBLIC ${CONFUSE_CFLAGS_OTHER})
target_link_libraries(h9d PUBLIC ${CONFUSE_LIBRARIES} PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog Python3::Python)

add_library(CLIENT_COMMON OBJECT ${CLIENT_COMMON_SRC} git_version.h)
target_include_directories(CLIENT_COMMON PUBLIC ${CONFUSE_INCLUDE_DIRS})

add_executable(h9sniffer ${H9SNIFFER_SRC} $<TARGET_OBJECTS:CLIENT_COMMON>)
target_include_directories(h9sniffer PUBLIC ${CONFUSE_INCLUDE_DIRS})
target_compile_options(h9sniffer PUBLIC ${CONFUSE_CFLAGS_OTHER})
target_link_libraries(h9sniffer PUBLIC ${CONFUSE_LIBRARIES} PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog)

add_executable(h9send ${H9SEND_SRC} $<TARGET_OBJECTS:CLIENT_COMMON>)
target_include_directories(h9send PUBLIC ${CONFUSE_INCLUDE_DIRS})
target_compile_options(h9send PUBLIC ${CONFUSE_CFLAGS_OTHER})
target_link_libraries(h9send PUBLIC ${CONFUSE_LIBRARIES} PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog)

add_executable(h9stats ${H9STATS_SRC} $<TARGET_OBJECTS:CLIENT_COMMON>)
target_include_directories(h9stats PUBLIC ${CONFUSE_INCLUDE_DIRS})
target_compile_options(h9stats PUBLIC ${CONFUSE_CFLAGS_OTHER})
target_link_libraries(h9stats PUBLIC ${CONFUSE_LIBRARIES} PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog)
