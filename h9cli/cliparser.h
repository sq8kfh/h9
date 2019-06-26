/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-26.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */
#ifndef H9_CLIPARSER_H
#define H9_CLIPARSER_H

#include "config.h"

#include "parser.hh"

// Give Flex the prototype of yylex we want ...
#define YY_DECL yy::parser::symbol_type yylex(CLIParser& cli_parser)
YY_DECL;

class CLIParser {
public:
    int result;
    void parse(const char *str);
    void scan_string(const char *str);
};


#endif //H9_CLIPARSER_H
