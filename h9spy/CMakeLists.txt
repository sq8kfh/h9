add_executable(h9spy h9spy.cc $<TARGET_OBJECTS:BUS> $<TARGET_OBJECTS:PROTOCOL> $<TARGET_OBJECTS:CTXCLIENT>)
target_link_libraries(h9spy ${confuse_LIBRARIES} ${libxml2_LIBRARIES})
