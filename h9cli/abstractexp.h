/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-29.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_ABSTRACTEXP_H
#define H9_ABSTRACTEXP_H

#include "config.h"
#include "commandctx.h"


class AbstractExp {
public:
public:
    virtual bool is_command() {
        return false;
    }
    virtual void operator()(CommandCtx* ctx) {};
    virtual const char** get_completion_list() = 0;
    virtual ~AbstractExp() {};
};


#endif //H9_ABSTRACTEXP_H
