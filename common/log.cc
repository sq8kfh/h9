/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-14.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "log.h"
#include <ctime>
#include <iostream>
#include <signal.h>
#include "logger.h"


static void handle_sighup(int signum) {
    if (signum == SIGHUP) {
        Logger::default_log.on_logrotate();
    }
}

Log::Log(): _verbose(Log::NOTICE), _debug(false), _to_stderr(false), _print_date(false), logfile("") {
}

Log::~Log() {
    ofs.close();
}

void Log::redirect_to_file(std::string filename) {
    _print_date = true;
    logfile = std::move(filename);
    ofs.open (logfile, std::ofstream::out | std::ofstream::app);
    std::clog.rdbuf(ofs.rdbuf());
    signal(SIGHUP, handle_sighup);
}

void Log::on_logrotate() {
    if (!logfile.empty()) {
        warn(__FILE__, __LINE__, "Logrotate");
        std::clog.flush();
        ofs.close();
        redirect_to_file(logfile);
    }
}

unsigned int Log::get_level() {
    return static_cast<unsigned int>(_verbose);
}

void Log::set_level(Log::Level level) {
    _verbose = level;
}

void Log::set_level(unsigned int level) {
    Log::Level tmp = static_cast<Log::Level>(level);
    if (tmp <= Log::Level::DEBUG2) {
        _verbose = tmp;
    }
    else {
        _verbose = Log::Level::DEBUG2;
    }
}

void Log::set_debug(bool debug) {
    _debug = debug;
}

void Log::set_to_stderr(bool to_stderr) {
    _to_stderr = to_stderr;
}

void Log::stderr(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::STDERR, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::stderr(const char *file_name, int line_number, const std::string &msg) const {
    log(Level::STDERR, file_name, line_number, msg);
}

void Log::crit(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::CRIT, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::crit(const char *file_name, int line_number, const std::string &msg) const {
    log(Level::CRIT, file_name, line_number, msg);
}

void Log::err(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::ERR, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::err(const char *file_name, int line_number, const std::string &msg) const {
    log(Level::ERR, file_name, line_number, msg);
}

void Log::warn(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::WARN, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::warn(const char *file_name, int line_number, const std::string &msg) const {
    log(Level::WARN, file_name, line_number, msg);
}

void Log::notice(const char *file_name, int line_number, const char* fmt, ...) const{
    va_list args;
    va_start(args, fmt);

    vlog(Level::NOTICE, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::notice(const char *file_name, int line_number, const std::string &msg) const {
    log(Level::NOTICE, file_name, line_number, msg);
}

void Log::info(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::INFO, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::info(const char *file_name, int line_number, const std::string &msg) const {
    log(Level::INFO, file_name, line_number, msg);
}

void Log::debug(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::DEBUG, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::debug(const char *file_name, int line_number, const std::string &msg) const {
    log(Level::DEBUG, file_name, line_number, msg);
}

void Log::debug2(const char *file_name, int line_number, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);

    vlog(Level::DEBUG2, file_name, line_number, fmt, args);

    va_end(args);
}

void Log::debug2(const char *file_name, int line_number, const std::string &msg) const {
    log(Level::DEBUG2, file_name, line_number, msg);
}

void Log::vlog(const Level& level, const char *file_name, int line_number, const char* fmt, va_list args) const {
    static size_t buf_len = 1024;
    static char* buf = new char[buf_len];

    va_list args_copy;
    va_copy(args_copy, args);
    while (std::vsnprintf(buf, buf_len, fmt, args_copy) >= buf_len) {
        log(Level::WARN, __FILE__, __LINE__, "a logger's buffer is too small - resizing");
        delete[] buf;
        buf_len *= 2;
        buf = new char[buf_len];
        va_copy(args_copy, args);
    }
    log(level, file_name, line_number, std::string(buf));
}

void Log::log(const Level& level, const char *file_name, int line_number, const std::string& msg) const {
    std::string tmp;
    if (_debug) {
        tmp += file_name;
        tmp += ':';
        tmp += std::to_string(line_number);
        tmp += ' ';
    }
    tmp += msg;
    log(level, tmp);
}

void Log::log(const std::string& msg) const {
    log(Level::NOTICE, msg);
}

void Log::log(const Log::Level& level, const std::string& msg) const {
    if (_verbose < level) {
        return;
    }
    if (_to_stderr || level == Level::STDERR) {
        std::cerr << msg << std::endl;
    }
    else {
        const char *level_name;
        switch(level) {
            case Level::CRIT: level_name = "CRITICAL"; break;
            case Level::ERR: level_name = "ERROR"; break;
            case Level::WARN: level_name = "WARNING"; break;
            case Level::NOTICE: level_name = "NOTICE"; break;
            case Level::INFO: level_name = "INFO"; break;
            case Level::DEBUG: level_name = "DEBUG"; break;
            case Level::DEBUG2: level_name = "DEBUG2"; break;
            case Level::STDERR: level_name = "STDERR"; break;
        }
        if (_print_date) {
            time_t now;
            time(&now);
            char buf[sizeof "1987-01-20T01:07:09Z"];
            strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
            std::clog << buf << " [" << level_name << "] " << msg << std::endl;
        }
        else {
            std::clog << '[' << level_name << "] " << msg << std::endl;
        }
    }
}
