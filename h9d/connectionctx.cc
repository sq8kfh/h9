/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "connectionctx.h"


ConnectionCtx::ConnectionCtx(std::uint16_t assigned_source_id): assigned_source_id(assigned_source_id) {

}

std::uint16_t ConnectionCtx::get_source_id(void) const {
    return assigned_source_id;
}
