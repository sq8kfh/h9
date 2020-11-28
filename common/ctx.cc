/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-17.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include <ctime>
#include <utility>
#include "ctx.h"
#include "config.h"
#include "logger.h"

void Ctx::raise_verbose_level(unsigned int how_much) {
    logger().set_level(logger().get_level() + how_much);
}

Ctx::Ctx(std::string app_name, std::string app_desc):
        _debug(false),
        _app_name(std::move(app_name)),
        _app_desc(std::move(app_desc)),
        start_time(std::time(nullptr)) {
    logger().set_debug(_debug);
    _devices_description_filename = std::string(H9_CONFIG_PATH) + "devices.conf";
}

void Ctx::enable_debug(bool debug) {
#ifdef H9_DEBUG
    _debug = debug;
#else
    _debug = false;
#endif
    logger().set_debug(debug);
}

Log& Ctx::logger() {
    return Logger::default_log;
}

/*Log& Ctx::log(const std::string& log_name) {
    return Logger::default_log;
}*/

time_t Ctx::get_start_time() {
    return start_time;
}

std::string Ctx::get_app_name() {
    return _app_name;
}

std::string Ctx::get_devices_description_filename() {
    return _devices_description_filename;
}
