add_executable(h9cli h9cli.cc $<TARGET_OBJECTS:CTXCLIENT>)
TARGET_LINK_LIBRARIES(h9cli ${confuse_LIBRARIES})