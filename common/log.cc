#include "log.h"
#include <iostream>

void Log::stderr(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::STDERR, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::crit(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::CRIT, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::err(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::ERR, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::warn(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::WARN, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::notice(const char *file_name, int line_number, const char* fmt, ...) const{
    va_list args;
    va_start(args, fmt);

    vlog(Level::NOTICE, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::info(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::INFO, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::debug(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::DEBUG, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::vlog(const Level& level, const char *file_name, int line_number, const char* fmt, va_list args) const {
    static size_t buf_len = 100;
    static char* buf = new char[buf_len];

    while (std::vsnprintf(buf, buf_len, fmt, args) >= buf_len) {
        log(Level::WARN, __FILE__, __LINE__, "a logger's buffer is too small - resizing");
        delete[] buf;
        buf_len *= 2;
        buf = new char[buf_len];
    }
    log(level, file_name, line_number, std::string(buf));
}

void Log::log(const Level& level, const char *file_name, int line_number, const std::string& msg) const {
    std::string tmp = {file_name};
    tmp += ':';
    tmp += std::to_string(line_number);
    tmp += ' ';
    tmp += msg;
    log(level, tmp);
}

void Log::log(const std::string& msg) const {
    log(Level::NOTICE, msg);
}

void Log::log(const Log::Level& level, const std::string& msg) const {
    std::cout << msg << std::endl;

    /*if (_verbose < level) {
        return;
    }

    FILE *out = stdout;
    if (_all_to_stderr || level == H9_LOG_STDERR) {
        out = stderr;
    }

    switch (level) {
        case H9_LOG_STDERR:
            break;
        default:
            fprintf(out, "%-6s ", level_name[level]);
            break;
    }
    if (_debug) {
        fprintf(out, "%s:%d: ", file, line_num);
    }
    FILE *out = stdout;
    vfprintf(out, fmt, args);*/
}
