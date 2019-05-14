#ifndef _H9_LOGGER_H_
#define _H9_LOGGER_H_

#include "common/log.h"

class Logger {
public:
    static Log default_log;
};

#define h9_log_stderr(fmt, ...) Logger::default_log.stderr(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_crit(fmt, ...) Logger::default_log.crit(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_err(fmt, ...) Logger::default_log.err(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_warn(fmt, ...) Logger::default_log.warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_notice(fmt, ...) Logger::default_log.notice(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_info(fmt, ...) Logger::default_log.info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_debug(fmt, ...) Logger::default_log.debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif //_H9_LOGGER_H_
