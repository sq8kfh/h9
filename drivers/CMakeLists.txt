SET(DRIVERS dummy.cc dummy.h loop.cc loop.h slcan.cc slcan.h)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    message("-- Enabled SocketCAN driver")
    list(APPEND DRIVERS socketcan.cc socketcan.h)
else()
    message("-- Disabled SocketCAN driver")
endif()

add_library(DRIVERS OBJECT ${DRIVERS})
