/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-30.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_ABSTRACTCOMMAND_H
#define H9_ABSTRACTCOMMAND_H

#include "config.h"
#include "abstractexp.h"


class AbstractCommand: public AbstractExp {
public:
    bool is_command() {
        return true;
    }
    const char** get_completion_list() {
        return nullptr;
    }
    virtual void operator()() = 0;
};


#endif //H9_ABSTRACTCOMMAND_H
