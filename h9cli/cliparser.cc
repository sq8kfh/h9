/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-26.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "cliparser.h"

void CLIParser::parse(const char *str) {
    scan_string(str);
    yy::parser parser(*this);
    parser.operator()();
}
