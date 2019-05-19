/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <cstdlib>

#include "common/clientctx.h"

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9cli", "Command line interface to the H9.");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    return EXIT_SUCCESS;
}
