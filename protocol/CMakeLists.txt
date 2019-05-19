#set(PROTOCOL ../bus/h9frame.cc ../bus/h9frame.h ../protocol/genericmsg.cc ../protocol/genericmsg.h ../protocol/concretizemsg.h ../protocol/framereceivedmsg.cc ../protocol/framereceivedmsg.h ../protocol/sendframemsg.cc ../protocol/sendframemsg.h ../protocol/h9connector.cc ../protocol/h9connector.h ../protocol/framemsg.h ../protocol/subscribemsg.cc ../protocol/subscribemsg.h ../protocol/errormsg.cc ../protocol/errormsg.h PARENT_SCOPE)

add_library(PROTOCOL OBJECT h9msg.xsd.h genericmsg.cc genericmsg.h concretizemsg.h framereceivedmsg.cc framereceivedmsg.h sendframemsg.cc sendframemsg.h h9connector.cc h9connector.h framemsg.h subscribemsg.cc ../protocol/subscribemsg.h ../protocol/errormsg.cc ../protocol/errormsg.h)

add_custom_command (
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        COMMAND xxd -i h9msg.xsd ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        DEPENDS h9msg.xsd
)
