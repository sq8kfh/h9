if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    message("-! Using epoll")
    SET(H9D_EVENT epoll.cc epoll.h)
    SET(H9D_EPOLL true)

    message("-! Enable SocketCAN driver")
    SET(H9_SOCKETCAN_DRIVER socketcan_driver.cc socketcan_driver.h)
else()
    message("-! Using kqueue")
    SET(H9D_EVENT kqueue.cc kqueue.h)
    SET(H9D_EPOLL false)

    message("-! Disable SocketCAN driver")
    SET(H9_SOCKETCAN_DRIVER "")
endif()

configure_file(config.h.in config.h)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

set(SRC h9d.cc config.h
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
        h9framecomparator.cc h9framecomparator.h)

add_custom_command(OUTPUT git_version.h  COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/generate_git_version_header.py > git_version.h DEPENDS ${SRC})

add_executable(h9d ${SRC} git_version.h)
target_include_directories(h9d PUBLIC ${CONFUSE_INCLUDE_DIRS})
target_compile_options(h9d PUBLIC ${CONFUSE_CFLAGS_OTHER})
target_link_libraries(h9d PUBLIC ${CONFUSE_LIBRARIES} PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog)
#target_link_libraries(example PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
