/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-26.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "cliparser.h"

const char* CLIParser::completion_list[] = {
    "node",
    nullptr
};

void CLIParser::clean_up() {
    if (result == nullptr) {
        delete last_match;
    }
    delete result;
    last_match = nullptr;
    result = nullptr;
}

CLIParser::CLIParser(bool print_parse_error): print_parse_error(print_parse_error), result(nullptr), last_match(nullptr) {
}

CLIParser::~CLIParser() {
    clean_up();
}

int CLIParser::parse(const char *str) {
    clean_up();
    scan_string(str);
    yy::parser parser(*this);
    return parser.operator()();
}

int CLIParser::parse_file(std::string file) {
    scan_begin(file);
    yy::parser parser(*this);
    int ret =  parser.operator()();
    scan_end();
    return ret;
}

const char ** CLIParser::get_completion_list() {
    return completion_list;
}
