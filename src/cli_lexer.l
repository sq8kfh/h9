%{
#include <jsonrpcpp/jsonrpcpp.hpp>
#include "cli_parsing_driver.h"
%}

%option noyywrap nounput noinput batch

/*%option debug*/

%%

#.*                     /*comment*/

[ \t\n]                 /*white space*/

;|\n                    return cli_drv.tee_token(yy::parser::make_LF_SEMICOLON());

"-"                     return cli_drv.tee_token(yy::parser::make_MINUS());
"+"                     return cli_drv.tee_token(yy::parser::make_PLUS());
"*"                     return cli_drv.tee_token(yy::parser::make_STAR());
"/"                     return cli_drv.tee_token(yy::parser::make_SLASH());
"("                     return cli_drv.tee_token(yy::parser::make_LPAREN());
")"                     return cli_drv.tee_token(yy::parser::make_RPAREN());

"="                     return cli_drv.tee_token(yy::parser::make_ASSIGN());
":"                     return cli_drv.tee_token(yy::parser::make_COLON());

node                    {
                            //printf("!- node token mach\n");
                            return cli_drv.tee_token(yy::parser::make_T_NODE());
                        }

restart                 {
                            //printf("!- restart token mach\n");
                            return cli_drv.tee_token(yy::parser::make_T_RESTART());
                        }

reg                     {
                            //printf("!- reg token mach\n");
                            return cli_drv.tee_token(yy::parser::make_T_REG());
                        }

set                     {
                            //printf("!- set token mach\n");
                            return cli_drv.tee_token(yy::parser::make_T_SET());
                        }

get                     {
                            //printf("!- get token mach\n");
                            return cli_drv.tee_token(yy::parser::make_T_GET());
                        }

setbit                  {
                            //printf("!- setbit token mach\n");
                            return cli_drv.tee_token(yy::parser::make_T_SETBIT());
                        }

clearbit                {
                            //printf("!- clearbit token mach\n");
                            return cli_drv.tee_token(yy::parser::make_T_CLEARBIT());
                        }

togglebit               {
                            //printf("!- togglebit token mach\n");
                            return cli_drv.tee_token(yy::parser::make_T_TOGGLEBIT());
                        }

[0-9]+                  {
                            //printf("!- number token mach\n");
                            int tmp = std::stoi(yytext, nullptr, 10);
                            //std::cout << "int: " << tmp << std::endl;
                            return cli_drv.tee_token(yy::parser::make_NUMBER(tmp));
                        }

b[01]{1,32}             {
                            int tmp = std::stoul(&yytext[1], nullptr, 2);
                            //std::cout << "int: " << tmp << std::endl;
                            return cli_drv.tee_token(yy::parser::make_NUMBER(tmp));
                        }

0[xX][0-9a-fA-F]{1,8}   {
                            int tmp = std::stoul(yytext, nullptr, 16);
                            //std::cout << "int: " << tmp << std::endl;
                            return cli_drv.tee_token(yy::parser::make_NUMBER(tmp));
                        }

\"[^\n\"]+\"            {
                            std::string val = {yytext};
                            val.erase(0, 1);
                            val.pop_back();
                            //std::cout << "string: " << val << '\n';
                            return cli_drv.tee_token(yy::parser::make_STRING(val));
                        }

.|[^ \t\n]+             {
                            throw yy::parser::syntax_error(std::string("invalid input: ") + yytext);
                        }

%%

void CLIParsingDriver::scan_string(const char *str) {
    YY_BUFFER_STATE bp = yy_scan_string(str);
    yy_switch_to_buffer(bp);
}

/*
void CLIParser::scan_begin(std::string file) {
    //yy_flex_debug = trace_scanning;
    if (file.empty () || file == "-")
        yyin = stdin;
    else if (!(yyin = fopen (file.c_str (), "r"))) {
        std::cerr << "cannot open " << file << ": " << strerror(errno) << '\n';
        exit (EXIT_FAILURE);
    }
}

void CLIParser::scan_end() {
    fclose (yyin);
}
*/