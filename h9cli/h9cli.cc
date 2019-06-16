/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "config.h"
#include <cstdlib>
#include <cstdio>

#include <readline/readline.h>
#include <readline/history.h>

#include "common/clientctx.h"


int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9cli", "Command line interface to the H9.");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    while (1) {
        // Display prompt and read input
        char* input = readline("h9> ");

        // Check for EOF.
        if (!input)
            break;

        // Add input to readline history.
        add_history(input);

        // Do stuff...

        // Free buffer that was allocated by readline
        free(input);
    }

    return EXIT_SUCCESS;
}
