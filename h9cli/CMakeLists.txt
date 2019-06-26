bison_target(h9cli_parser parser.yy parser.cc)
flex_target(h9cli_scaner lexer.ll lexer.cc)
#add_flex_bison_dependency(h9cli_scaner h9cli_parser)

add_executable(h9cli h9cli.cc ${FLEX_h9cli_scaner_OUTPUTS} ${BISON_h9cli_parser_OUTPUTS} $<TARGET_OBJECTS:CTXCLIENT> cliparser.cc cliparser.h)

target_include_directories(h9cli PUBLIC ${CONFUSE_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${READLINE_INCLUDE_DIRS})
target_compile_options(h9cli PUBLIC ${CONFUSE_CFLAGS_OTHER} ${LIBXML2_CFLAGS_OTHER} ${READLINE_CFLAGS_OTHER})
target_link_libraries(h9cli ${CONFUSE_LIBRARIES} ${LIBXML2_LIBRARIES} ${READLINE_LIBRARIES})
