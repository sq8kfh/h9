/*
 * H9 project
 *
 * Created by crowx on 2023-10-20.
 *
 * Copyright (C) 2023 Kamil Pałkowski. All rights reserved.
 */

#include <cstdio>
#include <readline/history.h>
#include <readline/readline.h>
#include <spdlog/spdlog.h>

#include "cli_parsing_driver.h"
#include "h9_configurator.h"

class H9CliConfigurator: public H9Configurator {
  private:
    void add_app_specific_opt() {
        // clang-format off
//        options.add_options("")
//                ("e,extended", "Extended output")
//                ("s,simple", "Simple output")
//                ;
        // clang-format on
    }

    //    void parse_app_specific_opt(const cxxopts::ParseResult& result) {
    //        extended = result.count("extended");
    //        simple = result.count("simple");
    //    }

  public:
    H9CliConfigurator():
        H9Configurator("h9cli", "Command line interface to the H9.") {}
};

static int quote_detector(char *line, int index) {
    //printf("quote_detector %s %d\n",line, index);
    return index > 0 && line[index - 1] == '\\' && !quote_detector(line, index - 1);
}

void write_readline_history() {
    std::string const HOME = std::getenv("HOME") ? std::getenv("HOME") : ".";
    write_history((HOME + "/.h9cli_history").c_str());
}

int main(int argc, char** argv) {
    H9CliConfigurator h9;
    h9.logger_initial_setup();
    h9.parse_command_line_arg(argc, argv);
    h9.logger_setup();
    h9.load_configuration();

    H9Connector h9_connector = h9.get_connector();

    try {
        h9_connector.connect("h9cli");
    }
    catch (std::system_error& e) {
        SPDLOG_ERROR("Can not connect to h9bus {}:{}: {}.", h9.get_host(), h9.get_port(), e.code().message());
        exit(EXIT_FAILURE);
    }
    catch (std::runtime_error& e) {
        SPDLOG_ERROR("Can not connect to h9bus {}:{}: {}.", h9.get_host(), h9.get_port(), e.what());
        exit(EXIT_FAILURE);
    }

    CLIParsingDriver& cli = CLIParsingDriver::create(&h9_connector);

    //rl_completer_word_break_characters = " \t\n\\'`@$><=;|&{(";
    rl_attempted_completion_function = cli_completion_wrapper;
    rl_completer_quote_characters = "\"'";
    rl_char_is_quoted_p = &quote_detector;
    rl_completer_word_break_characters = " ";
    //    rl_special_prefixes = "\"";
    //    rl_basic_quote_characters = "";

    rl_sort_completion_matches = 1;

    std::string prompt = "\033[38;5;172mh9cli \033[m@\033[38;5;153m" + h9_connector.hostname() + "\033[m> ";

    std::string const HOME = std::getenv("HOME") ? std::getenv("HOME") : ".";
    read_history((HOME + "/.h9cli_history").c_str());
    atexit(write_readline_history);

    while (true) {
        char* input = readline(prompt.c_str());

        if (!input)
            break;

        if (strlen(input))
            add_history(input);

        write_readline_history();
        int parse_res = cli.parse(input);

        if (parse_res == 0) {
            cli.execute();
        }
        // exec_unit(cli_parser.result, &cmd_ctx);

        free(input);
    }
}
