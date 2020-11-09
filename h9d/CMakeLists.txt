add_executable(h9d h9d.cc $<TARGET_OBJECTS:BUS> $<TARGET_OBJECTS:PROTOCOL> $<TARGET_OBJECTS:CTXDAEMON> node.cc node.h bus.cc bus.h connectionctx.cc connectionctx.h dctx.cc dctx.h)
target_include_directories(h9d PUBLIC ${CONFUSE_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
target_compile_options(h9d PUBLIC ${CONFUSE_CFLAGS_OTHER} ${LIBXML2_CFLAGS_OTHER})
target_link_libraries(h9d ${CONFUSE_LIBRARIES} ${LIBXML2_LIBRARIES})
