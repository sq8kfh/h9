/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-28.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "devicedescloader.h"
#include "common/logger.h"


static void cfg_err_func(cfg_t *cfg, const char* fmt, va_list args) {
    Logger::default_log.vlog(Log::Level::WARN, __FILE__, __LINE__, fmt, args);
}

DeviceDescLoader::DeviceDescLoader(): cfg(nullptr) {
}

DeviceDescLoader::~DeviceDescLoader() {
    if (cfg) {
        cfg_free(cfg);
    }
}

void DeviceDescLoader::load_file(std::string devices_desc_file) {
    cfg_opt_t cfg_register_sec[] = {
            CFG_STR("name", nullptr, CFGF_NONE),
            CFG_STR("type", nullptr, CFGF_NONE),
            CFG_INT("size", 0, CFGF_NONE),
            CFG_STR("mode", nullptr, CFGF_NONE),
            CFG_BOOL("readable", cfg_false, CFGF_NONE),
            CFG_BOOL("writable", cfg_false, CFGF_NONE),
            CFG_STR("description", "", CFGF_NONE),
            CFG_END()
    };

    cfg_opt_t cfg_device_sec[] = {
            CFG_STR("name", nullptr, CFGF_NONE),
            CFG_STR("description", "", CFGF_NONE),
            CFG_SEC("register", cfg_register_sec, CFGF_MULTI | CFGF_TITLE | CFGF_NO_TITLE_DUPES),
            CFG_END()
    };

    cfg_opt_t cfg_opts[] = {
            CFG_SEC("device", cfg_device_sec, CFGF_MULTI | CFGF_TITLE | CFGF_NO_TITLE_DUPES),
            CFG_END()
    };

    cfg = cfg_init(cfg_opts, CFGF_NONE);
    cfg_set_error_function(cfg, cfg_err_func);
    int ret = cfg_parse(cfg, devices_desc_file.c_str());

    if (ret == CFG_FILE_ERROR) {
        h9_log_err("Devices description file (%s) - file error", devices_desc_file.c_str());
        cfg_free(cfg);
        return;
    } else if (ret == CFG_PARSE_ERROR) {
        h9_log_err("Devices description file (%s) - parse error", devices_desc_file.c_str());
        cfg_free(cfg);
        return;
    }

    int dn = cfg_size(cfg, "device");
    for (int di = 0; di < dn; ++di) {
        cfg_t *device = cfg_getnsec(cfg, "device", di);
        int device_type = std::stoul(cfg_title(device));

        auto &desc = devices[device_type];
        desc.name = std::string(cfg_getstr(device, "name"));
        desc.description = std::string(cfg_getstr(device, "description"));

        int rn = cfg_size(device, "register");
        for (int ri = 0; ri < rn; ++ri) {
            cfg_t *register_c = cfg_getnsec(device, "register", ri);
            int req_num = std::stoul(cfg_title(register_c));

            if (req_num < 10 || req_num > 0xff) continue; //skip reg number below 10 ang higher then 255

            auto &reg = desc.registers[req_num];
            reg.number = req_num;
            reg.name = std::string(cfg_getstr(register_c, "name"));
            reg.type = std::string(cfg_getstr(register_c, "type"));
            reg.size = cfg_getint(register_c, "size");
            reg.readable = cfg_getbool(register_c, "readable");
            reg.writable = cfg_getbool(register_c, "writable");
            reg.description = std::string(cfg_getstr(register_c, "description"));
        }
    }
}

std::string DeviceDescLoader::get_device_name_by_type(std::uint16_t type) {
    if (devices.count(type)) {
        return devices[type].name;
    }
    return "";
}

std::string DeviceDescLoader::get_device_description_by_type(std::uint16_t type) {
    if (devices.count(type)) {
        return devices[type].description;
    }
    return "";
}

std::map<std::uint16_t, DeviceDescLoader::RegisterDesc> DeviceDescLoader::get_device_register_by_type(std::uint16_t type) {
    if (devices.count(type)) {
        return devices[type].registers;
    }
    return std::map<std::uint16_t, DeviceDescLoader::RegisterDesc>();
}
