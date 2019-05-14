add_library(ctxclient STATIC ctx.h ctx.cc clientctx.h clientctx.cc logger.cc logger.h log.cc log.h)
add_library(ctxdaemon STATIC ctx.h ctx.cc daemonctx.h daemonctx.cc logger.cc logger.h log.cc log.h)
#target_include_directories(ctx PUBLIC .)

#target_link_libraries(ctx)