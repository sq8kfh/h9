set(H9FWU_FILES h9fwu.cc)

add_executable(h9fwu ${H9FWU_FILES} ${PROTOCOL})
TARGET_LINK_LIBRARIES(h9fwu ${libxml2_LIBRARIES} ctxclient)
