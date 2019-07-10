%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.3.2"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include "abstractexp.h"
    #include "abstractcommand.h"
    #include "expression.h"
    class CLIParser;
}

%param { CLIParser& cli_parser }

//%locations

%define parse.trace
%define parse.error verbose

%code {
	#include "cliparser.h"
}

%define api.token.prefix {TOK_}

%token END  0      "end of file"
%token LF_SEMICOLON
%token T_NODE      "node"
%token T_RESTART   "restart"
%token T_REG       "reg"
%token T_SET1      "set1"
%token T_SET2      "set2"
%token T_SET3      "set3"
%token T_SET4      "set4"
%token T_GET       "get"
%token MINUS       "-"
%token PLUS        "+"
%token STAR        "*"
%token SLASH       "/"
%token LPAREN      "("
%token RPAREN      ")"
%token ASSIGN      "="
%token <std::string> STRING
%token <int> NUMBER "number"
%type  <int> num_exp
%type  <NodeExp*> node_exp
%type  <NodeRegExp*> node_reg_exp
%type  <AbstractCommand*> node_command
%type  <AbstractCommand*> node_reg_command

//%destructor { printf ("Discarding node_exp symbol.\n"); delete $$; } node_exp
//%destructor { printf ("Discarding node_reg_exp symbol.\n"); delete $$; } node_reg_exp
//%destructor { printf ("Discarding NodeRegExp symbol.\n"); } <NodeRegExp*>
//%destructor { printf ("Discarding AbstractCommand symbol.\n"); } <AbstractCommand*>

//%printer { yyo << $$; } <*>;

%%

%start unit;
unit:
        %empty
        | node_exp             	{ cli_parser.result = $1; }
        | node_command         	{ cli_parser.result = $1; }
	| node_reg_exp         	{ cli_parser.result = $1; }
	| node_reg_command	{ cli_parser.result = $1; }

node_exp:
	"node" num_exp			{ $$ = new NodeExp($2); cli_parser.last_match = $$; }

node_command:
        node_exp "restart"      	{ $$ = new NodeRestart($1); cli_parser.last_match = $$; }

node_reg_exp:
	node_exp "reg" num_exp  	{ $$ = new NodeRegExp($1, $3); cli_parser.last_match = $$; }
	//| node_exp error                  { printf("error\n"); }

node_reg_command:
	node_reg_exp "get"      	{ $$ = new NodeGetReg($1); cli_parser.last_match = $$; }
	| node_reg_exp "set1" num_exp	{ $$ = new NodeSet1Reg($1, $3); cli_parser.last_match = $$; }
	| node_reg_exp "set2" num_exp	{ $$ = new NodeSet2Reg($1, $3); cli_parser.last_match = $$; }
	| node_reg_exp "set3" num_exp	{ $$ = new NodeSet3Reg($1, $3); cli_parser.last_match = $$; }
	| node_reg_exp "set4" num_exp	{ $$ = new NodeSet4Reg($1, $3); cli_parser.last_match = $$; }

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
    if (cli_parser.print_parse_error)
    	std::cerr << ": " << m << '\n';
}
