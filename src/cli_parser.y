%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.0"
%language "c++"
%header

%define api.token.raw
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    //#include "abstractexp.h"
    //#include "abstractcommand.h"
    //#include "expression.h"
    class CLIParsingDriver;
}

%param { CLIParsingDriver& cli_drv }

//%locations

%define parse.trace
%define parse.error verbose
%define parse.lac full

%code {
	#include <cli_parsing_driver.h>
}

%define api.token.prefix {TOK_}

%token END  0      "end of file"
%token LF_SEMICOLON
%token MINUS       "-"
%token PLUS        "+"
%token STAR        "*"
%token SLASH       "/"
%token LPAREN      "("
%token RPAREN      ")"
%token ASSIGN      "="
%token COLON       ":"
%token <std::string> STRING
%token <int> NUMBER "number"
%nterm  <int> num_exp

//%destructor { printf ("Discarding node_exp symbol.\n"); delete $$; } node_exp
//%destructor { printf ("Discarding node_reg_exp symbol.\n"); delete $$; } node_reg_exp
//%destructor { printf ("Discarding NodeRegExp symbol.\n"); } <NodeRegExp*>
//%destructor { printf ("Discarding AbstractCommand symbol.\n"); } <AbstractCommand*>

//%printer { yyo << $$; } <*>;

%%

%start num_exp;

%left "+" "-";
%left "*" "/";

num_exp:
	NUMBER
	| num_exp "+" num_exp   { $$ = $1 + $3; }
	| num_exp "-" num_exp   { $$ = $1 - $3; }
	| num_exp "*" num_exp   { $$ = $1 * $3; }
	| num_exp "/" num_exp   { $$ = $1 / $3; }
	| "(" num_exp ")"   	{ $$ = $2; }
	| "-" num_exp           { $$ = -$2; }

%%

void yy::parser::error (const std::string& m) {
    //if (cli_parser.print_parse_error)
    	std::cerr << ": " << m << '\n';
}
