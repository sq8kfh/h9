add_executable(h9sniffer h9sniffer.cc $<TARGET_OBJECTS:BUS> $<TARGET_OBJECTS:PROTOCOL> $<TARGET_OBJECTS:CTXCLIENT>)
target_include_directories(h9sniffer PUBLIC ${CONFUSE_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
target_compile_options(h9sniffer PUBLIC ${CONFUSE_CFLAGS_OTHER} ${LIBXML2_CFLAGS_OTHER})
target_link_libraries(h9sniffer ${CONFUSE_LIBRARIES} ${LIBXML2_LIBRARIES})
