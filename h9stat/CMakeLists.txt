add_executable(h9stat h9stat.cc $<TARGET_OBJECTS:BUS> $<TARGET_OBJECTS:PROTOCOL> $<TARGET_OBJECTS:CTXCLIENT>)
target_link_libraries(h9stat ${confuse_LIBRARIES} ${libxml2_LIBRARIES})
