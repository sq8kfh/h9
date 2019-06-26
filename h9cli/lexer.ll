%{
#include "parser.hh"
#include "cliparser.h"
%}

%option noyywrap nounput noinput batch debug

%{
  // Code run each time a pattern is matched.
  //# define YY_USER_ACTION  loc.columns (yyleng);


%}

%%

%{
  // A handy shortcut to the location held by the driver.
  //yy::location& loc = drv.location;
  // Code run each time yylex is called.
  //loc.step ();
%}

#.*                     /*comment*/

[ \t]                   /*white space*/

[a-z]+                  {
                            return yy::parser::make_KEYWORD();
                        }

-?[0-9]+                {
                            int tmp = std::stoi(yytext, nullptr, 10);
                            std::cout << "int: " << tmp << std::endl;
                            return yy::parser::make_NUMBER(tmp);
                        }

b[01]{1,32}             {
                            int tmp = std::stoul(&yytext[1], nullptr, 2);
                            std::cout << "int: " << tmp << std::endl;
                            return yy::parser::make_NUMBER(tmp);
                        }

0[xX][0-9a-fA-F]{1,8}   {
                            int tmp = std::stoul(yytext, nullptr, 16);
                            std::cout << "int: " << tmp << std::endl;
                            return yy::parser::make_NUMBER(tmp);
                        }

\"[^\n\"]+\"            {
                            std::string val = {yytext};
                            val.erase(0, 1);
                            val.pop_back();
                            std::cout << "string: " << val << '\n';
                            return yy::parser::make_STRING(val);
                        }

.                       {
                            std::cerr << "invalid input: " << yytext << '\n';
                            return yy::parser::make_END();
                        }

%%

void CLIParser::scan_string(const char *str) {
    YY_BUFFER_STATE bp = yy_scan_string(str);
    yy_switch_to_buffer(bp);

}

/*int yyFlexLexer::yywrap(void) {
    return 1;
}*/
