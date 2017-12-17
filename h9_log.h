#ifndef _H9_LOG_H_
#define _H9_LOG_H_

#include <stdarg.h>

#define H9_LOG_STDERR  0
#define H9_LOG_CRIT    1
#define H9_LOG_ERR     2
#define H9_LOG_WARN    3
#define H9_LOG_NOTICE  4
#define H9_LOG_INFO    5
#define H9_LOG_DEBUG   6

#define h9_log_stderr(fmt, ...) h9_log_write(H9_LOG_STDERR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_crit(fmt, ...) h9_log_write(H9_LOG_CRIT, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_err(fmt, ...) h9_log_write(H9_LOG_ERR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_warn(fmt, ...) h9_log_write(H9_LOG_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_notice(fmt, ...) h9_log_write(H9_LOG_NOTICE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_info(fmt, ...) h9_log_write(H9_LOG_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define h9_log_debug(fmt, ...) h9_log_write(H9_LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

void h9_log_init(int verbose, int debug, int all_to_stderr);
void h9_log_set_verbose(int level);
void h9_log_write(unsigned int level, const char *file, int line_num, const char *fmt, ...);
void h9_log_vwrite(unsigned int level, const char *file, int line_num, const char *fmt, va_list args);

#endif //_H9_LOG_H_
