/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_CONNECTIONCTX_H
#define H9_CONNECTIONCTX_H

#include "config.h"
#include <cstdint>

class ConnectionCtx {
private:
    const std::uint16_t assigned_source_id;
public:
    ConnectionCtx(std::uint16_t assigned_source_id);
    std::uint16_t get_source_id(void) const;
};


#endif //H9_CONNECTIONCTX_H
