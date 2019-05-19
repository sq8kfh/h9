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
        _debug(false),
        _app_name(std::move(app_name)),
        _app_desc(std::move(app_desc)) {

    log().set_debug(_debug);
}

void Ctx::enable_debug(bool debug) {
#ifdef H9_DEBUG
    _debug = debug;
#else
    _debug = false;
#endif
    log().set_debug(debug);
}

inline bool Ctx::is_debug() {
    return _debug;
}

Log& Ctx::log() {
    return Logger::default_log;
}

Log& Ctx::log(const std::string& log_name) {
    return Logger::default_log;
}
