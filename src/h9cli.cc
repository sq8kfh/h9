/*
 * H9 project
 *
 * Created by crowx on 2023-10-20.
 *
 * Copyright (C) 2023 Kamil Pałkowski. All rights reserved.
 */

#include <cstdio>
#include <readline/history.h>
#include <readline/readline.h>

#include "cli_parsing_driver.h"

int main(int argc, char** argv) {
    CLIParsingDriver cli;
    while (true) {
        char* input = readline("h9> ");

        if (!input)
            break;

        if (strlen(input))
            add_history(input);

        // write_readline_history();
        int parse_res = cli.parse(input);
        if (parse_res)
            continue;
        // exec_unit(cli_parser.result, &cmd_ctx);

        free(input);
    }
}
