set(H9SPY_FILES h9spy.cc)
set(PROTOCOL ../bus/h9frame.cc ../bus/h9frame.h ../protocol/genericmsg.cc ../protocol/genericmsg.h ../protocol/concretizemsg.h ../protocol/framereceivedmsg.cc ../protocol/framereceivedmsg.h ../protocol/sendframemsg.cc ../protocol/sendframemsg.h ../protocol/h9connector.cc ../protocol/h9connector.h ../protocol/framemsg.h)

add_executable(h9spy ${H9SPY_FILES} ${PROTOCOL})
TARGET_LINK_LIBRARIES(h9spy ${libxml2_LIBRARIES}  ctxclient)