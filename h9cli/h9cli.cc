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

#include <readline/readline.h>
#include <readline/history.h>
#include <FlexLexer.h>

#include "common/clientctx.h"
#include "cliparser.h"

char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

const char *character_names[] = {
        "node",
        "reg",
        "write",
        "read",
        nullptr
};

char **
character_name_completion(const char *text, int start, int end)
{
    printf("rl_attempted_completion_function: (%s, %d, %d0\n", text, start, end);
    printf("<%s>\n", rl_line_buffer);

    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, character_name_generator);
}

char *
character_name_generator(const char *text, int state)
{
    printf("character_name_generator (%s, %d)\n", text, state);
    static int list_index, len;
    const char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = character_names[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

class Node {
public:
    void operator()();
    Node operator[](int a);
};

//Command["node"](12)["reg"](10)["write"](0x12)
//                              ["read"]()
//                   ["restart"]

int main(int argc, char **argv)
{
    ClientCtx ctx = ClientCtx("h9cli", "Command line interface to the H9.");

    auto res = ctx.parse_options(argc, argv);
    ctx.load_configuration(res);

    rl_attempted_completion_function = character_name_completion;

    std::stringstream ss;
    while (true) {
        // Display prompt and read input
        char* input = readline("h9> ");

        // Check for EOF.
        if (!input)
            break;

        // Add input to readline history.
        if (strlen(input)) add_history(input);
        //ss.str(input);
        //yyFlexLexer lexer = {&ss};

        int yv;
        CLIParser cli_parser;
        cli_parser.parse(input);
        //parser.
        //auto a = yylex(cli_parser);
        /*while ((yv=lexer.yylex()) != 0) {
            printf(": %d\n", yv);
            //40            std::cout << " yylex() " << yv << " yylval.dval " << yylval.dval << std::endl;
            //41            t0.value=yylval.dval;
            //42            Parse (pParser, yv, t0);

        }*/


        // Do stuff...

        // Free buffer that was allocated by readline
        free(input);
    }

    return EXIT_SUCCESS;
}
