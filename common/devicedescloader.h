/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-28.
 *
 * Copyright (C) 2020-2021 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_DEVICEDESCLOADER_H
#define H9_DEVICEDESCLOADER_H

#include "config.h"
#include <string>
#include <map>
#include <vector>
#include <confuse.h>


class DeviceDescLoader {
public:
    struct RegisterDesc {
        std::uint8_t number;
        std::string name;
        std::string type;
        int size;
        bool readable;
        bool writable;
        std::vector<std::string> bits_names;
        std::string description;
    };

    struct DeviceDesc {
        std::string name;
        std::string description;
        std::map<std::uint16_t, RegisterDesc> registers;
    };

private:
    cfg_t *cfg;
    std::map<std::uint16_t, DeviceDesc> devices;
public:
    DeviceDescLoader();
    ~DeviceDescLoader();
    void load_file(std::string devices_desc_file);

    std::string get_device_name_by_type(std::uint16_t type);
    std::string get_device_description_by_type(std::uint16_t type);
    std::map<std::uint16_t, RegisterDesc> get_device_register_by_type(std::uint16_t type);
};


#endif //H9_DEVICEDESCLOADER_H
