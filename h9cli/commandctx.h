/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-30.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_COMMANDCTX_H
#define H9_COMMANDCTX_H

#include "config.h"
#include "protocol/h9connector.h"

class CommandCtx {
private:
    H9Connector* h9_connector;
    std::uint16_t source_id;
public:
    CommandCtx(H9Connector* h9_connector, std::uint16_t source_id);

    H9Connector* get_connector();
    std::uint16_t get_source_id();
};


#endif //H9_COMMANDCTX_H
