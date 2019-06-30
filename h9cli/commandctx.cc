/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-30.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "commandctx.h"

CommandCtx::CommandCtx(H9Connector* h9_connector, std::uint16_t source_id): h9_connector(h9_connector), source_id(source_id) {

}

H9Connector* CommandCtx::get_connector() {
    return h9_connector;
}

std::uint16_t CommandCtx::get_source_id() {
    return source_id;
}
