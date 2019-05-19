/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-17.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_CTX_H_
#define _H9_CTX_H_

#include "config.h"
#include <string>

#include "common/log.h"


class Ctx {
private:
    bool _debug;
protected:
    const std::string _app_name;
    const std::string _app_desc;
    void raise_verbose_level(unsigned int how_much);
public:
    Ctx(std::string app_name, std::string app_desc);
    void enable_debug(bool debug);
    inline bool is_debug();

    Log& log();
    Log& log(const std::string& log_name);
};


#endif //_H9_CTX_H_
