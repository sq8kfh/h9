/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-17.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_CTX_H_
#define _H9_CTX_H_

#include "config.h"
#include <string>

#include "common/log.h"


class Ctx {
private:
    bool _debug;
    const time_t start_time;
protected:
    const std::string _app_name;
    const std::string _app_desc;
    void raise_verbose_level(unsigned int how_much);
    Ctx(std::string app_name, std::string app_desc);
    void enable_debug(bool debug);
public:
    bool cfg_debug() {
        return _debug;
    };

    Log& logger();
    //Log& log(const std::string& log_name);
    time_t get_start_time();
};


#endif //_H9_CTX_H_
