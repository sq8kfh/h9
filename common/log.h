/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-14.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_LOG_H_
#define _H9_LOG_H_

#include "config.h"
#include <string>
#include <cstdarg>


class Log {
public:
    enum class Level : unsigned int {
        STDERR = 0,
        CRIT   = 1,
        ERR    = 2,
        WARN   = 3,
        NOTICE = 4,
        INFO   = 5,
        DEBUG  = 6
    };

    constexpr static Level CRIT = Level::CRIT;
    constexpr static Level ERR = Level::ERR;
    constexpr static Level WARN = Level::WARN;
    constexpr static Level NOTICE = Level::NOTICE;
    constexpr static Level INFO = Level::INFO;
    constexpr static Level DEBUG = Level::DEBUG;
private:
    Level _verbose;
    bool _debug;
    bool _to_stderr;
public:
    Log();
    unsigned int get_level();
    void set_level(Level level);
    void set_level(unsigned int  level);
    void set_debug(bool debug);
    void set_to_stderr(bool to_stderr);

    void stderr(const char *file_name, int line_number, const char* fmt, ...) const;
    void stderr(const char *file_name, int line_number, const std::string &msg) const;
    void crit(const char *file_name, int line_number, const char* fmt, ...) const;
    void crit(const char *file_name, int line_number, const std::string &msg) const;
    void err(const char *file_name, int line_number, const char* fmt, ...) const;
    void err(const char *file_name, int line_number, const std::string &msg) const;
    void warn(const char *file_name, int line_number, const char* fmt, ...) const;
    void warn(const char *file_name, int line_number, const std::string &msg) const;
    void notice(const char *file_name, int line_number, const char* fmt, ...) const;
    void notice(const char *file_name, int line_number, const std::string &msg) const;
    void info(const char *file_name, int line_number, const char* fmt, ...) const;
    void info(const char *file_name, int line_number, const std::string &msg) const;
    void debug(const char *file_name, int line_number, const char* fmt, ...) const;
    void debug(const char *file_name, int line_number, const std::string &msg) const;

    void vlog(const Level& level, const char *file_name, int line_number, const char* fmt, va_list args) const;

    void log(const Level& level, const char *file_name, int line_number, const std::string& msg) const;
    void log(const std::string& msg) const;

    void log(const Level& level, const std::string& msg) const;
};


#endif //_H9_LOG_H_
