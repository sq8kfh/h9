find_package(LibXml2 REQUIRED)

pkg_check_modules(confuse libconfuse)
pkg_check_modules(libxml2 libxml-2.0)

set(H9BUS_FILES h9bus.cc)

add_executable(h9bus ${H9BUS_FILES})
TARGET_LINK_LIBRARIES(h9bus ${confuse_LIBRARIES} ${libxml2_LIBRARIES})