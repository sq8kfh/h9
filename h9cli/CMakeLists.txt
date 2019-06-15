add_executable(h9cli h9cli.cc $<TARGET_OBJECTS:CTXCLIENT>)
target_link_libraries(h9cli ${confuse_LIBRARIES})