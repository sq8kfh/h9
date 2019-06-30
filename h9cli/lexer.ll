%{
#include "parser.hh"
#include "cliparser.h"
%}

%option noyywrap nounput noinput batch
/*%option debug*/

%%

#.*                     /*comment*/

[ \t\n]                 /*white space*/

"-"                     return yy::parser::make_MINUS();
"+"                     return yy::parser::make_PLUS();
"*"                     return yy::parser::make_STAR();
"/"                     return yy::parser::make_SLASH();
"("                     return yy::parser::make_LPAREN();
")"                     return yy::parser::make_RPAREN();

"="                     return yy::parser::make_ASSIGN();

node                    return yy::parser::make_T_NODE();
restart                 return yy::parser::make_T_RESTART();
reg                     return yy::parser::make_T_REG();
set                     return yy::parser::make_T_SET();
get                     return yy::parser::make_T_GET();

[0-9]+                  {
                            int tmp = std::stoi(yytext, nullptr, 10);
                            //std::cout << "int: " << tmp << std::endl;
                            return yy::parser::make_NUMBER(tmp);
                        }

b[01]{1,32}             {
                            int tmp = std::stoul(&yytext[1], nullptr, 2);
                            //std::cout << "int: " << tmp << std::endl;
                            return yy::parser::make_NUMBER(tmp);
                        }

0[xX][0-9a-fA-F]{1,8}   {
                            int tmp = std::stoul(yytext, nullptr, 16);
                            //std::cout << "int: " << tmp << std::endl;
                            return yy::parser::make_NUMBER(tmp);
                        }

\"[^\n\"]+\"            {
                            std::string val = {yytext};
                            val.erase(0, 1);
                            val.pop_back();
                            //std::cout << "string: " << val << '\n';
                            return yy::parser::make_STRING(val);
                        }

.|[^ \t\n]+             {
                            std::cerr << "invalid input: " << yytext << '\n';
                            return yy::parser::make_END();
                        }

<<EOF>>                 return yy::parser::make_END();

%%

void CLIParser::scan_string(const char *str) {
    YY_BUFFER_STATE bp = yy_scan_string(str);
    yy_switch_to_buffer(bp);

}

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
