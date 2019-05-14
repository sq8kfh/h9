#include <utility>
#include "ctx.h"
#include "config.h"
#include "logger.h"

void Ctx::raise_verbose_level(unsigned int how_much) {
    _verbose += how_much;
}

Ctx::Ctx(const std::string& app_name, const std::string& app_desc):
        _app_name(app_name),
        _app_desc(app_desc),
#ifdef H9_DEBUG
        _debug(false),
#endif
        _verbose(0) {

}

inline unsigned int Ctx::verbose_level() {
    return _verbose;
}

inline bool Ctx::debug_enabled() {
#ifdef H9_DEBUG
    return _debug;
#else
    return false;
#endif
}

Log& Ctx::log() {
    return Logger::default_log;
}

Log& Ctx::log(const std::string& log_name) {
    return Logger::default_log;
}
