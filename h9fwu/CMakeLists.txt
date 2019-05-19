set(H9FWU_FILES h9fwu.cc)
set(PROTOCOL ../bus/h9frame.cc ../bus/h9frame.h ../protocol/genericmsg.cc ../protocol/genericmsg.h ../protocol/concretizemsg.h ../protocol/framereceivedmsg.cc ../protocol/framereceivedmsg.h ../protocol/sendframemsg.cc ../protocol/sendframemsg.h ../protocol/h9connector.cc ../protocol/h9connector.h ../protocol/framemsg.h ../protocol/subscribemsg.cc ../protocol/subscribemsg.h)

add_executable(h9fwu ${H9FWU_FILES} ${PROTOCOL})
TARGET_LINK_LIBRARIES(h9fwu ${libxml2_LIBRARIES} ctxclient)