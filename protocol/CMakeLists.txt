add_library(PROTOCOL OBJECT
        h9msg.xsd.h
        genericmsg.cc
        genericmsg.h
        concretizemsg.h
        framereceivedmsg.cc
        framereceivedmsg.h
        sendframemsg.cc
        sendframemsg.h
        h9connector.cc
        h9connector.h
        framemsg.h
        subscribemsg.cc
        subscribemsg.h
        errormsg.cc
        errormsg.h
        methodcallmsg.cc
        methodcallmsg.h
        methodresponsemsg.cc
        methodresponsemsg.h
        genericmethod.h)

add_custom_command (
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        COMMAND xxd -i h9msg.xsd ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        DEPENDS h9msg.xsd
)
