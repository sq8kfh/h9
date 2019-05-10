add_library(ctxclient STATIC ctx.h ctx.cc clientctx.h clientctx.cc)
add_library(ctxdaemon STATIC ctx.h ctx.cc daemonctx.h daemonctx.cc)
#target_include_directories(ctx PUBLIC .)

#target_link_libraries(ctx)