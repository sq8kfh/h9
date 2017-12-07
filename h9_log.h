#ifndef _H9_LOG_H_
#define _H9_LOG_H_

#include <stdarg.h>

#define H9_LOG_STDERR  0
#define H9_LOG_EMERG   1
#define H9_LOG_CRIT    2
#define H9_LOG_ERR     3
#define H9_LOG_WARN    4
#define H9_LOG_NOTICE  5
#define H9_LOG_INFO    6
#define H9_LOG_DEBUG   7

#define h9_log_stderr(fmt, ...) h9_log_write(H9_LOG_STDERR, fmt, ##__VA_ARGS__)
#define h9_log_emerg(fmt, ...) h9_log_write(H9_LOG_EMERG, fmt, __VA_ARGS__)
#define h9_log_crit(fmt, ...) h9_log_write(H9_LOG_CRIT, fmt, __VA_ARGS__)
#define h9_log_err(fmt, ...) h9_log_write(H9_LOG_ERR, fmt, __VA_ARGS__)
#define h9_log_warn(fmt, ...) h9_log_write(H9_LOG_WARN, fmt, __VA_ARGS__)
#define h9_log_notice(fmt, ...) h9_log_write(H9_LOG_NOTICE, fmt, __VA_ARGS__)
#define h9_log_info(fmt, ...) h9_log_write(H9_LOG_INFO, fmt, __VA_ARGS__)
#define h9_log_debug(fmt, ...) h9_log_write(H9_LOG_DEBUG, fmt, __VA_ARGS__)

void h9_log_init(void);
void h9_log_write(unsigned int level, const char *fmt, ...);
void h9_log_vwrite(unsigned int level, const char *fmt, va_list args);

#endif //_H9_LOG_H_
