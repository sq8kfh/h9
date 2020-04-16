add_executable(h9bus h9bus.cc busmgr.cc busmgr.h cfg.cc cfg.h socketmgr.cc socketmgr.h eventmgr.cc eventmgr.h driver.cc driver.h servermgr.cc servermgr.h tcpclient.cc tcpclient.h tcpserver.cc tcpserver.h  $<TARGET_OBJECTS:BUS> $<TARGET_OBJECTS:PROTOCOL> $<TARGET_OBJECTS:DRIVERS> $<TARGET_OBJECTS:CTXDAEMON> framelogger.cc framelogger.h busctx.cc busctx.h)
target_include_directories(h9bus PUBLIC ${CONFUSE_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
target_compile_options(h9bus PUBLIC ${CONFUSE_CFLAGS_OTHER} ${LIBXML2_CFLAGS_OTHER})
target_link_libraries(h9bus ${CONFUSE_LIBRARIES} ${LIBXML2_LIBRARIES})
