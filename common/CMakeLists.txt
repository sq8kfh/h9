add_library(CTXCLIENT OBJECT ctx.h ctx.cc clientctx.h clientctx.cc logger.cc logger.h log.cc log.h devicedescloader.cc devicedescloader.h)
add_library(CTXDAEMON OBJECT ctx.h ctx.cc daemonctx.h daemonctx.cc logger.cc logger.h log.cc log.h devicedescloader.cc devicedescloader.h h9tuple.cc h9tuple.h h9value.cc h9value.h)
