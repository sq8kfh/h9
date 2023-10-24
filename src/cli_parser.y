%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.0"
%language "c++"
%header

%define api.token.raw
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include <jsonrpcpp/jsonrpcpp.hpp>
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
	#include "cli_parsing_driver.h"
}

%define api.token.prefix {TOK_}

%token END  0      "end of file"
%token LF_SEMICOLON

%token T_NODE      "node"
%token T_RESTART   "restart"
%token T_REG       "reg"
%token T_SET       "set"
%token T_GET       "get"
%token T_SETBIT    "setbit"
%token T_CLEARBIT  "clearbit"
%token T_TOGGLEBIT "togglebit"

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
%nterm <int> num_exp

%nterm  <nlohmann::json> node_exp node_sub_exp
%nterm  <jsonrpcpp::Request> node_command
%nterm  <nlohmann::json> node_reg_exp
%nterm  <jsonrpcpp::Request> node_reg_command

%printer { yyo << $$.c_str(); } STRING
%printer { yyo << $$.dump().c_str(); } node_exp
%printer { yyo << $$.dump().c_str(); } node_sub_exp

//%destructor { printf ("Discarding node_exp symbol.\n"); delete $$; } node_exp
//%destructor { printf ("Discarding node_reg_exp symbol.\n"); delete $$; } node_reg_exp
//%destructor { printf ("Discarding NodeRegExp symbol.\n"); } <NodeRegExp*>
//%destructor { printf ("Discarding AbstractCommand symbol.\n"); } <AbstractCommand*>

//%printer { yyo << $$; } <*>;

%%

%start unit;

%left "+" "-";
%left "*" "/";

unit:
    %empty                              { printf("!! empty\n"); }
    | node_command                      { printf("!! %s\n", $1.to_json().dump().c_str()); }
    | node_reg_command                  { printf("!! %s\n", $1.to_json().dump().c_str()); }
    | num_exp                           { printf("!! %d\n", $1); }

node_exp:
    node_sub_exp                        {   $$ = $1; }

node_sub_exp:
	"node" num_exp			            {
	                                        printf("!! node num_exp\n");
	                                        $$ = nlohmann::json({{"dev_id", $2}});
	                                    }

node_command:
    node_exp "restart"      	        {
                                            printf("!! node_exp restart\n");
                                            jsonrpcpp::Id id(1);
                                            $$ = jsonrpcpp::Request(id, "restart", $1);
                                        }

node_reg_exp:
    node_exp "reg" num_exp  	        {
                                            printf("!! node_exp reg num_exp\n");
                                            $1["reg"] = $3;
                                            $$ = $1;
                                        }
	//| node_exp error                  { printf("!! error\n"); }

node_reg_command:
	node_reg_exp "get"      	        {
                                            printf("!! node_exp get\n");
                                            jsonrpcpp::Id id(2);
                                            $$ = jsonrpcpp::Request(id, "get", $1);
                                        }
	| node_reg_exp "setbit" num_exp	    {
                                            printf("!! node_exp setbit num_exp\n");
                                            jsonrpcpp::Id id(2);
                                            $1["bit_num"] = $3;
                                            $$ = jsonrpcpp::Request(id, "setbit", $1);
                                        }
	| node_reg_exp "clearbit" num_exp	{
                                            printf("!! node_exp clearbit num_exp\n");
                                            jsonrpcpp::Id id(2);
                                            $1["bit_num"] = $3;
                                            $$ = jsonrpcpp::Request(id, "clearbit", $1);
                                        }
	| node_reg_exp "togglebit" num_exp	{
                                            printf("!! node_exp togglebit num_exp\n");
                                            jsonrpcpp::Id id(2);
                                            $1["bit_num"] = $3;
                                            $$ = jsonrpcpp::Request(id, "togglebit", $1);
                                        }
	| node_reg_exp "set" num_exp	    {
                                            printf("!! node_exp set num_exp\n");
                                            jsonrpcpp::Id id(2);
                                            $1["value"] = $3;
                                            $$ = jsonrpcpp::Request(id, "set", $1);
                                        }
	| node_reg_exp "set" STRING  	    {
                                            printf("!! node_exp set num_exp\n");
                                            jsonrpcpp::Id id(2);
                                            $1["value"] = $3;
                                            $$ = jsonrpcpp::Request(id, "set", $1);
                                        }
num_exp:
	NUMBER
	| num_exp "+" num_exp               { $$ = $1 + $3; }
	| num_exp "-" num_exp               { $$ = $1 - $3; }
	| num_exp "*" num_exp               { $$ = $1 * $3; }
	| num_exp "/" num_exp               { $$ = $1 / $3; }
	| "(" num_exp ")"   	            { $$ = $2; }
	| "-" num_exp                       { $$ = -$2; }

%%

void yy::parser::error (const std::string& m) {
    //if (cli_parser.print_parse_error)
 /*   	std::cerr << ": " << m << '\n';

        yy_stack_print_();
        printf("yytable[]: %c\n", yytable_[yylast_]);
        printf("term num: %d\n", yyfinal_);
        for (auto& a : yylac_stack_) {
            printf("%c ", a);
        }
        printf("\n");

            *yycdebug_ << "Stack3 now";
            for (stack_type::const_iterator
                   i = yystack_.begin (),
                   i_end = yystack_.end ();
                 i != i_end; ++i)
              *yycdebug_ << ' ' << i->name();
            *yycdebug_ << '\n';
*/
}
