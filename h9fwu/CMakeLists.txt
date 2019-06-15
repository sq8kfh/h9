add_executable(h9fwu h9fwu.cc $<TARGET_OBJECTS:BUS> $<TARGET_OBJECTS:PROTOCOL> $<TARGET_OBJECTS:CTXCLIENT>)
target_link_libraries(h9fwu ${confuse_LIBRARIES} ${libxml2_LIBRARIES})
