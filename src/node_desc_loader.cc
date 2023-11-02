/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-28.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#include "node_desc_loader.h"

#include "spdlog/spdlog.h"
#include "libconfuse_helper.h"

NodeDescLoader::NodeDescLoader():
    cfg(nullptr) {
}

NodeDescLoader::~NodeDescLoader() {
    if (cfg) {
        cfg_free(cfg);
    }
}

void NodeDescLoader::load_file(const std::string& nodes_desc_file) {
    cfg_opt_t cfg_register_sec[] = {
        CFG_STR("name", nullptr, CFGF_NONE),
        CFG_STR("type", nullptr, CFGF_NONE),
        CFG_INT("size", 0, CFGF_NONE),
//        CFG_STR("mode", nullptr, CFGF_NONE),
        CFG_BOOL("readable", cfg_false, CFGF_NONE),
        CFG_BOOL("writable", cfg_false, CFGF_NONE),
        CFG_STR_LIST("bits-names", (char*)"{}", CFGF_NONE),
        CFG_STR("description", "", CFGF_NONE),
        CFG_END()};

    cfg_opt_t cfg_type_sec[] = {
        CFG_STR("name", nullptr, CFGF_NONE),
        CFG_STR("description", "", CFGF_NONE),
        CFG_SEC("register", cfg_register_sec, CFGF_MULTI | CFGF_TITLE | CFGF_NO_TITLE_DUPES),
        CFG_END()};

    cfg_opt_t cfg_opts[] = {
        CFG_SEC("node_type", cfg_type_sec, CFGF_MULTI | CFGF_TITLE | CFGF_NO_TITLE_DUPES),
        CFG_END()};

    cfg = cfg_init(cfg_opts, CFGF_NONE);
    cfg_set_error_function(cfg, confuse_helpers::cfg_err_func);
    cfg_set_validate_func(cfg, "node_type|register|type", confuse_helpers::validate_node_register_type);
    cfg_set_validate_func(cfg, "node_type|register|size", confuse_helpers::validate_node_register_size);
    cfg_set_validate_func(cfg, "node_type", confuse_helpers::validate_node_type_sec);
    cfg_set_validate_func(cfg, "node_type|register",confuse_helpers::validate_node_register_number_sec);
    int ret = cfg_parse(cfg, nodes_desc_file.c_str());

    if (ret == CFG_FILE_ERROR) {
        SPDLOG_ERROR("Nodes description file ({}) - file error", nodes_desc_file.c_str());
        cfg_free(cfg);
        return;
    }
    else if (ret == CFG_PARSE_ERROR) {
        SPDLOG_ERROR("Nodes description file ({}) - parse error", nodes_desc_file.c_str());
        cfg_free(cfg);
        return;
    }

    int dn = cfg_size(cfg, "node_type");
    for (int di = 0; di < dn; ++di) {
        cfg_t* device = cfg_getnsec(cfg, "node_type", di);
        int device_type = std::stoul(cfg_title(device));

        auto& desc = types[device_type];
        desc.name = std::string(cfg_getstr(device, "name"));
        desc.description = std::string(cfg_getstr(device, "description"));

        int rn = cfg_size(device, "register");
        for (int ri = 0; ri < rn; ++ri) {
            cfg_t* register_c = cfg_getnsec(device, "register", ri);
            int req_num = std::stoul(cfg_title(register_c));

            if (req_num < 10 || req_num > 0xff)
                continue; // skip reg number below 10 ang higher then 255

            auto& reg = desc.registers[req_num];
            reg.number = req_num;
            reg.name = std::string(cfg_getstr(register_c, "name"));
            reg.type = std::string(cfg_getstr(register_c, "type"));
            reg.size = cfg_getint(register_c, "size");
            reg.readable = cfg_getbool(register_c, "readable");
            reg.writable = cfg_getbool(register_c, "writable");
            for (int i = cfg_size(register_c, "bits-names") - 1; i >= 0; --i) {
                reg.bits_names.push_back(cfg_getnstr(register_c, "bits-names", i));
            }
            reg.description = std::string(cfg_getstr(register_c, "description"));
        }
    }
}

std::string NodeDescLoader::get_node_name_by_type(std::uint16_t type) {
    if (types.count(type)) {
        return types[type].name;
    }
    return "";
}

std::string NodeDescLoader::get_node_description_by_type(std::uint16_t type) {
    if (types.count(type)) {
        return types[type].description;
    }
    return "";
}

std::map<std::uint16_t, NodeDescLoader::RegisterDesc> NodeDescLoader::get_node_register_by_type(std::uint16_t type) {
    if (types.count(type)) {
        return types[type].registers;
    }
    return std::map<std::uint16_t, NodeDescLoader::RegisterDesc>();
}
