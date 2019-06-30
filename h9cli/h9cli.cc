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
#include <sstream>
#include <cstring>
#include <functional>

#include <readline/readline.h>
#include <readline/history.h>
#include <FlexLexer.h>

#include "common/clientctx.h"
#include "cliparser.h"

const char** completion_list = nullptr;

char* cli_completion_generator(const char *text, int state) {
    static int list_index, len;
    const char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    if (completion_list == nullptr)
        return nullptr;

    while ((name = completion_list[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return nullptr;
}

char** cli_completion(const char *text, int start, int end) {
    CLIParser cli_parser = {false};
    char *input = strndup(rl_line_buffer, start);
    int res = cli_parser.parse(input);
    free(input);

    //std::cout << cli_parser.last_match << std::endl;

    //printf("<%s>:<%s> %d %d %d\n", rl_line_buffer, text, start, end, res);

    if (res == 0) {
        if (cli_parser.last_match) {
            completion_list = cli_parser.last_match->get_completion_list();
        } else {
            completion_list = cli_parser.get_completion_list();
        }
    }
    else {
        completion_list = nullptr;
    }

    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, cli_completion_generator);
}

int main(int argc, char **argv) {
    ClientCtx ctx = ClientCtx("h9cli", "Command line interface to the H9.");
    ctx.add_positional_option("h9file", "<h9 file>", "");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);


    if (res.count("h9file")) {
        CLIParser cli_parser;

        FILE *fp = stdin;
        if (res["h9file"].as<std::string>() != "-") {
            fp = fopen(res["h9file"].as<std::string>().c_str(), "r");
            if (fp == nullptr)
                return EXIT_SUCCESS;
        }

        char* line = nullptr;
        size_t len = 0;
        while ((getline(&line, &len, fp)) != -1) {
            int res = cli_parser.parse(line);
            //std::cout << "result(" << res << "): "<< cli_parser.result << std::endl;
            if (cli_parser.result && cli_parser.result->is_command()) {
                cli_parser.result->operator()();
            }
        }
        fclose(fp);
        if (line)
            free(line);
        
        return EXIT_SUCCESS;
    }


    rl_attempted_completion_function = cli_completion;

    std::stringstream ss;
    CLIParser cli_parser;
    while (true) {
        char* input = readline("h9> ");

        // Check for EOF.
        if (!input)
            break;

        if (strlen(input)) add_history(input);


        int res = cli_parser.parse(input);
        //std::cout << "result(" << res << "): "<< cli_parser.result << std::endl;
        if (cli_parser.result && cli_parser.result->is_command()) {
            cli_parser.result->operator()();
        }

        // Free buffer that was allocated by readline
        free(input);
    }

    return EXIT_SUCCESS;
}
