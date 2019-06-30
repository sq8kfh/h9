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

#include <vector>
#include <string>

#include "parser.hh"
#include "abstractexp.h"

// Give Flex the prototype of yylex we want ...
#define YY_DECL yy::parser::symbol_type yylex(CLIParser& cli_parser)
YY_DECL;

class CLIParser {
private:
    static const char * completion_list[];
    void scan_string(const char *str);
    void scan_begin(std::string file);
    void scan_end();
    void clean_up();
public:
    const bool print_parse_error;
    CLIParser(bool print_parse_error = true);
    ~CLIParser();
    AbstractExp* result;
    AbstractExp* last_match;
    int parse(const char *str);
    int parse_file(std::string file);
    const char ** get_completion_list();
};


#endif //H9_CLIPARSER_H
