set(H9SPY_FILES h9spy.cc)

add_executable(h9spy ${H9SPY_FILES})
TARGET_LINK_LIBRARIES(h9spy ctxclient)