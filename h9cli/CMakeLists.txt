set(H9CLI_FILES h9cli.cc)

add_executable(h9cli ${H9CLI_FILES})
TARGET_LINK_LIBRARIES(h9cli ctxclient)