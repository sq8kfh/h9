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

add_executable(h9d h9d.cc config.h
               h9frame.cc h9frame.h
               ext_h9frame.cc ext_h9frame.h
               busframe.cc busframe.h
               bus_driver.cc bus_driver.h
               loop_driver.cc loop_driver.h
               slcan_driver.cc slcan_driver.h
               ${H9_SOCKETCAN_DRIVER}
               ${H9D_EVENT}
               bus.h
               framesubject.cc frameobserver.h
               frameobserver.cc frameobserver.h
               h9framecomparator.cc h9framecomparator.h
               )
target_include_directories(h9d PUBLIC ${CONFUSE_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
target_compile_options(h9d PUBLIC ${CONFUSE_CFLAGS_OTHER} ${LIBXML2_CFLAGS_OTHER})
target_link_libraries(h9d pthread ${CONFUSE_LIBRARIES} ${LIBXML2_LIBRARIES})
