add_executable(h9send h9send.cc $<TARGET_OBJECTS:BUS> $<TARGET_OBJECTS:PROTOCOL> $<TARGET_OBJECTS:CTXCLIENT>)
target_link_libraries(h9send ${confuse_LIBRARIES} ${libxml2_LIBRARIES})
