/*
 * Created by crowx on 21/10/2023.
 *
 */

#pragma once

#include "cli_parser.h"
#include "h9connector.h"

# define YY_DECL yy::parser::symbol_type yylex(CLIParsingDriver& cli_drv)
YY_DECL;

char** cli_completion_wrapper(const char *text, int start, int end);

class CLIParsingDriver {
  private:
    class HackParser: public yy::parser {
      public:
        std::vector<std::string> alt;

        HackParser(CLIParsingDriver& ref): parser(ref) {}

        virtual std::string yysyntax_error_(const context& yyctx) const override {
            symbol_kind_type yyarg[YYNTOKENS];
            int a = yyctx.expected_tokens(yyarg, YYNTOKENS);
            const_cast<HackParser*>(this)->alt.clear();

            for (int i = 0; i < a; ++i) {
                const_cast<HackParser*>(this)->alt.push_back(yy::parser::symbol_name(yyarg[i]));
                //printf("%s\n", yy::parser::symbol_name(yyarg[i]).c_str());
            }
            return std::string("HackParser");
        }
    };

    static CLIParsingDriver instance;
    H9Connector* h9d;

    CLIParsingDriver();

    HackParser parser;
    yy::parser::symbol_kind::symbol_kind_type last_token;

    void scan_string(const char *str);
  public:
    static CLIParsingDriver& get() {
        return instance;
    }

    void set_connector(H9Connector* connector);

    char** cli_completion(const char *text, int start, int end);
    char* cli_grammar_completion(const char *text, int state);
    char* cli_node_completion(const char *text, int state);

    yy::parser::symbol_type tee_token(yy::parser::symbol_type symbol);
    int parse(const char *str);

};
