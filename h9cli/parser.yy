%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.4.1"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    class CLIParser;
}

// The parsing context.
%param { CLIParser& cli_parser }


//%locations

%define parse.trace
%define parse.error verbose

%code {
	#include "cliparser.h"
}

%define api.token.prefix {TOK_}
%token END  0  "end of file"
%token KEYWORD;
%token <std::string> STRING
%token <int> NUMBER
%type  <int> exp

%printer { yyo << $$; } <*>;

%%

%start unit;
unit: exp { cli_parser.result = $1; };

%left "+" "-";
%left "*" "/";
exp:
NUMBER
| exp "+" exp   { $$ = $1 + $3; }
| exp "-" exp   { $$ = $1 - $3; }
| exp "*" exp   { $$ = $1 * $3; }
| exp "/" exp   { $$ = $1 / $3; }
| "(" exp ")"   { $$ = $2; }

%%

void yy::parser::error (const std::string& m) {
    std::cerr << ": " << m << '\n';
}
