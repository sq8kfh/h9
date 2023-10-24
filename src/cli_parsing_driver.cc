/*
 * Created by crowx on 21/10/2023.
 *
 */

#include "cli_parsing_driver.h"

#include <readline/readline.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

CLIParsingDriver CLIParsingDriver::instance;

char* cli_main_completion_generator(const char* text, int state) {
    static const char* completion_list[] = {"node", "h9d", nullptr};
    static int list_index, len;
    const char* name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = completion_list[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return nullptr;
}

char** cli_completion_wrapper(const char* text, int start, int end) {
    return CLIParsingDriver::get().cli_completion(text, start, end);
}

static char* cli_grammar_completion_wrapper(const char* text, int state) {
    return CLIParsingDriver::get().cli_grammar_completion(text, state);
}

static char* cli_node_completion_wrapper(const char* text, int state) {
    return CLIParsingDriver::get().cli_node_completion(text, state);
}

CLIParsingDriver::CLIParsingDriver():
    h9d(nullptr),
    parser(*this) {
}

void CLIParsingDriver::set_connector(H9Connector* connector) {
    h9d = connector;
}

char** CLIParsingDriver::cli_completion(const char* text, int start, int end) {
    printf("\n%s\n", rl_line_buffer);
    printf("%*c %d\n", start + 1, '^', start);
    printf("%*c %d\n", end + 1, '^', end);
    printf("%s\n", text);
    rl_attempted_completion_over = 1;
    if (start == 0)
        return rl_completion_matches(text, cli_main_completion_generator);
    else {
        char* input = strndup(rl_line_buffer, start);
//        printf("parsing \"%s\" ...\n", input);
        scan_string(input);
        free(input);
        parser.set_debug_level(0);
        parser.parse();

        if (last_token == yy::parser::symbol_kind::symbol_kind_type::S_T_NODE)
            return rl_completion_matches(text, cli_node_completion_wrapper);

        return rl_completion_matches(text, cli_grammar_completion_wrapper);
    }
    return nullptr;
}

char* CLIParsingDriver::cli_grammar_completion(const char* text, int state) {
    static int len;
    static decltype(parser.alt)::iterator it;

    if (!state) {
        it = parser.alt.begin();
        len = strlen(text);
    }

    while (it != parser.alt.end()) {
        if ((*it).compare(0, len, text) == 0) {
            return strdup((*it++).c_str());
        }
        it++;
    }
    return nullptr;
}

char* CLIParsingDriver::cli_node_completion(const char* text, int state) {
    static std::vector<std::string> completion_list;
    static int len;
    static decltype(completion_list)::iterator it;

    if (!state) {
        len = strlen(text);

        jsonrpcpp::Id id(h9d->get_next_id());

        jsonrpcpp::Request req(id, "get_devices_list");
        h9d->send(std::make_shared<jsonrpcpp::Request>(req));

        jsonrpcpp::entity_ptr raw_msg;
        try {
            raw_msg = h9d->recv();
        }
        catch (std::system_error& e) {
            SPDLOG_ERROR("Messages receiving error: {}.", e.code().message());
            exit(EXIT_FAILURE);
        }
        catch (std::runtime_error& e) {
            SPDLOG_ERROR("Messages receiving error: {}.", e.what());
            exit(EXIT_FAILURE);
        }

        completion_list.clear();
        if (raw_msg->is_response()) {
            jsonrpcpp::response_ptr msg = std::dynamic_pointer_cast<jsonrpcpp::Response>(raw_msg);
            auto j = msg->result();
            for (auto& dev : j) {
                completion_list.push_back("\"" + dev["name"].get<std::string>() + "\"");
                //std::cout << dev["name"] << '\n';
            }
        }

        it = completion_list.begin();
    }

    if (state == 0 && text[0] == '\0') {
        // printf("node <%s>:%d %s\n", text, state, "<node ID>");
        return strdup("<node ID>");
    }

    while (it != completion_list.end()) {
        if ((*it).compare(0, len, text) == 0) {
            return strdup((*it++).c_str());
        }
        it++;
    }
    return nullptr;
}

yy::parser::symbol_type CLIParsingDriver::tee_token(yy::parser::symbol_type symbol) {
    last_token = symbol.type_get();
    return std::move(symbol);
}

int CLIParsingDriver::parse(const char* str) {
    scan_string(str);
    parser.set_debug_level(10);
    return parser.parse();
}
