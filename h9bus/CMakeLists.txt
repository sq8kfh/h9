find_package(LibXml2 REQUIRED)

pkg_check_modules(confuse libconfuse)
pkg_check_modules(libxml2 libxml-2.0)

add_executable(h9bus h9bus.cc busmgr.cc busmgr.h cfg.cc cfg.h socketmgr.cc socketmgr.h eventmgr.cc eventmgr.h driver.cc driver.h servermgr.cc servermgr.h tcpclient.cc tcpclient.h tcpserver.cc tcpserver.h  $<TARGET_OBJECTS:BUS> $<TARGET_OBJECTS:PROTOCOL> $<TARGET_OBJECTS:DRIVERS> $<TARGET_OBJECTS:CTXDAEMON>)
TARGET_LINK_LIBRARIES(h9bus ${confuse_LIBRARIES} ${libxml2_LIBRARIES})
