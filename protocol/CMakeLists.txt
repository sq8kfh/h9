add_library(PROTOCOL OBJECT
        h9msg.xsd.h
        genericmsg.cc
        genericmsg.h
        concretizemsg.h
        framemsg.cc
        framemsg.h
        sendframemsg.cc
        sendframemsg.h
        h9connector.cc
        h9connector.h
        genericframemsg.h
        subscribemsg.cc
        subscribemsg.h
        errormsg.cc
        errormsg.h
        executemethodmsg.cc
        executemethodmsg.h
        methodresponsemsg.cc
        methodresponsemsg.h
        genericmethod.h
        value.cc value.h h9socket.cc h9socket.h h9msgsocket.cc h9msgsocket.h identificationmsg.cc identificationmsg.h executedevicemethodmsg.cc executedevicemethodmsg.h devicemethodresponsemsg.cc devicemethodresponsemsg.h deviceevent.cc deviceevent.h)

target_include_directories(PROTOCOL PUBLIC ${LIBXML2_INCLUDE_DIRS})
target_compile_options(PROTOCOL PUBLIC ${LIBXML2_CFLAGS_OTHER})

add_custom_command (
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        COMMAND ${XXD_EXECUTABLE} -i h9msg.xsd ${CMAKE_CURRENT_BINARY_DIR}/h9msg.xsd.h
        DEPENDS h9msg.xsd
)
