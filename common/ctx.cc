#include <utility>

#include <utility>

#include <utility>
#include "ctx.h"
#include "config.h"
#include "logger.h"

void Ctx::raise_verbose_level(unsigned int how_much) {
    log().set_level(log().get_level() + how_much);
}

Ctx::Ctx(std::string app_name, std::string app_desc):
#ifdef H9_DEBUG
        _debug(false),
#endif
        _app_name(std::move(app_name)),
        _app_desc(std::move(app_desc)) {

    log().set_debug(_debug);
}

void Ctx::enable_debug(bool debug) {
    _debug = debug;
    log().set_debug(debug);
}

inline bool Ctx::is_debug() {
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
