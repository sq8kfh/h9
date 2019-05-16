find_package(LibXml2 REQUIRED)

pkg_check_modules(confuse libconfuse)
pkg_check_modules(libxml2 libxml-2.0)

set(H9BUS_FILES h9bus.cc busmgr.cc busmgr.h cfg.cc cfg.h socketmgr.cc socketmgr.h driver.cc driver.h ../bus/h9frame.cc ../bus/h9frame.h servermgr.cc servermgr.h tcpclient.cc tcpclient.h)
set(DRIVERS ../drivers/dummy.cc ../drivers/dummy.h ../drivers/loop.cc ../drivers/loop.h ../drivers/slcan.cc ../drivers/slcan.h ../drivers/socketcan.cc ../drivers/socketcan.h tcpserver.cc tcpserver.h)
set(PROTOCOL ../protocol/genericmsg.cc ../protocol/genericmsg.h ../protocol/concretizemsg.h ../protocol/framereceivedmsg.cc ../protocol/framereceivedmsg.h ../protocol/sendframemsg.cc ../protocol/sendframemsg.h)

add_executable(h9bus ${H9BUS_FILES} ${DRIVERS} ${PROTOCOL})
TARGET_LINK_LIBRARIES(h9bus ${confuse_LIBRARIES} ${libxml2_LIBRARIES} ctxdaemon)