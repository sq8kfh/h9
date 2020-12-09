if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    message("-- Using epoll")
    SET(H9D_EVENT epoll.cc epoll.h)
else()
    message("-- Using kqueue")
    SET(H9D_EVENT kqueue.cc kqueue.h)
endif()


add_executable(h9d h9d.cc $<TARGET_OBJECTS:BUS> $<TARGET_OBJECTS:PROTOCOL> $<TARGET_OBJECTS:CTXDAEMON> $<TARGET_OBJECTS:DEVICES> node.cc node.h bus.cc bus.h connectionctx.cc connectionctx.h dctx.cc dctx.h frameobserver.cc frameobserver.h tcpclientthread.cc tcpclientthread.h tcpserver.cc tcpserver.h devmgr.cc devmgr.h executor.cc executor.h executoradapter.cc executoradapter.h ${H9D_EVENT} framesubject.cc framesubject.h device.cc device.h sharedmutex.cc sharedmutex.h)
target_include_directories(h9d PUBLIC ${CONFUSE_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
target_compile_options(h9d PUBLIC ${CONFUSE_CFLAGS_OTHER} ${LIBXML2_CFLAGS_OTHER})
target_link_libraries(h9d pthread ${CONFUSE_LIBRARIES} ${LIBXML2_LIBRARIES})
