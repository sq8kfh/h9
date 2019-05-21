SET(DRIVERS dummy.cc dummy.h loop.cc loop.h slcan.cc slcan.h)

if(LINUX)
    message("-- Enabled SocketCAN driver")
    list(APPEND DRIVERS socketcan.cc socketcan.h)
else()
    message("-- Disabled SocketCAN driver")
endif()

add_library(DRIVERS OBJECT ${DRIVERS})
