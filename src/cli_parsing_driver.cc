/*
 * Created by crowx on 21/10/2023.
 *
 */

#include "cli_parsing_driver.h"

int CLIParsingDriver::parse(const char *str) {
    //clean_up();
    scan_string(str);
    yy::parser parser(*this);
    return parser.operator()();
}
