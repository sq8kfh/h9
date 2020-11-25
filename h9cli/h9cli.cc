/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-09.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
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
#include "commandctx.h"


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

void exec_unit(AbstractExp* unit, CommandCtx* cmd_ctx) {
    //std::cout << "result(" << res << "): "<< cli_parser.result << std::endl;
    if (unit && unit->is_command()) {
        unit->operator()(cmd_ctx);
    }
}

void write_readline_history() {
    std::string const HOME = std::getenv("HOME") ? std::getenv("HOME") : ".";
    write_history((HOME + "/.h9cli_history").c_str());
}

int main(int argc, char **argv) {
    ClientCtx ctx = ClientCtx("h9cli", "Command line interface to the H9.");
    ctx.add_options("s,src_id", "Source id", cxxopts::value<std::uint16_t>());
    ctx.add_positional_option("h9file", "<h9 file>", "");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);


    std::uint16_t source_id = ctx.get_default_source_id();

    if (res.count("src_id")) {
        source_id = res["src_id"].as<std::uint16_t>();
    }

    H9Connector h9_connector = {ctx.get_h9bus_host(), ctx.get_h9bus_port()};

    if (h9_connector.connect(ctx.get_app_name()) == -1) {
        return EXIT_FAILURE;
    }

    CommandCtx cmd_ctx = {&h9_connector, source_id};
    CLIParser cli_parser;

    if (res.count("h9file")) {
        FILE *fp = stdin;
        if (res["h9file"].as<std::string>() != "-") {
            fp = fopen(res["h9file"].as<std::string>().c_str(), "r");
            if (fp == nullptr) return EXIT_FAILURE;
        }

        char* line = nullptr;
        size_t len = 0;

        while ((getline(&line, &len, fp)) != -1) {
            int parse_res = cli_parser.parse(line);
            if (parse_res) break;
            exec_unit(cli_parser.result, &cmd_ctx);
        }
        fclose(fp);
        if (line) {
            free(line);
        }
    }
    else {
        rl_attempted_completion_function = cli_completion;

        std::string const HOME = std::getenv("HOME") ? std::getenv("HOME") : ".";
        read_history((HOME + "/.h9cli_history").c_str());
        atexit(write_readline_history);

        std::stringstream ss;
        while (true) {
            char *input = readline("h9> ");

            if (!input) break;

            if (strlen(input)) add_history(input);

            write_readline_history();
            int parse_res = cli_parser.parse(input);
            if(parse_res) continue;
            exec_unit(cli_parser.result, &cmd_ctx);

            free(input);
        }
    }
    return EXIT_SUCCESS;
}
