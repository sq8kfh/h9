SET(DEVICES antennaswitch.cc antennaswitch.h)

add_library(DEVICES OBJECT ${DEVICES})
target_include_directories(DEVICES PUBLIC ${LIBXML2_INCLUDE_DIRS})
target_compile_options(DEVICES PUBLIC ${LIBXML2_CFLAGS_OTHER})

