/*
 * Created by crowx on 21/10/2023.
 *
 */

#pragma once

#include "cli_parser.h"

# define YY_DECL yy::parser::symbol_type yylex(CLIParsingDriver& cli_drv)
YY_DECL;

class CLIParsingDriver {
  private:
    void scan_string(const char *str);
  public:
    int parse(const char *str);
};
