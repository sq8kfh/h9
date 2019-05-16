set(H9SEND_FILES h9send.cc)
set(PROTOCOL ../bus/h9frame.cc ../bus/h9frame.h ../protocol/genericmsg.cc ../protocol/genericmsg.h ../protocol/concretizemsg.h ../protocol/framereceivedmsg.cc ../protocol/framereceivedmsg.h ../protocol/sendframemsg.cc ../protocol/sendframemsg.h ../protocol/h9connector.cc ../protocol/h9connector.h)

add_executable(h9send ${H9SEND_FILES} ${PROTOCOL})
TARGET_LINK_LIBRARIES(h9send ${libxml2_LIBRARIES} ctxclient)