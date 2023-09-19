/*
 * Created by crowx on 18/09/2023.
 *
 */

#include "metrics_collector.h"

#include <iostream>
#include <regex>

void to_json(nlohmann::json& j, const MetricsCollector::counter_t& f) {
    MetricsCollector::counter_t::value_type tmp = f;
    j = nlohmann::json(tmp);
}

MetricsCollector MetricsCollector::_this;

MetricsCollector::MetricsCollector(): start_time(std::time(nullptr)) {
    std::cout << test_counter.is_lock_free() << std::endl;
    std::cout << test_counter.is_always_lock_free << std::endl;
}

nlohmann::json MetricsCollector::metrics_to_json() {
    nlohmann::json j({{"uptime", std::time(nullptr) - _this.start_time}});

    std::regex sprit_regex("\\.");
    std::regex array_regex("([A-Za-z0-9_-]+)(?:\\[([A-Za-z0-9_-]+)=([A-Za-z0-9_-]+)\\])?");
    for (const auto& [name, value] : _this.counter_map) {
        nlohmann::json* tmp = &j;
        for (const auto& t : std::vector<std::string>(std::sregex_token_iterator(name.begin(), name.end(), sprit_regex, -1), std::sregex_token_iterator())) {
            std::smatch match;
            if (std::regex_search(t, match, array_regex)) {
                std::string tag = match[1];

                if (match[3].matched) {
                    std::string arg = match[2];
                    std::string arg_value = match[3];

                    nlohmann::json* m = tmp;
                    if (tmp->contains(tag)) {
                        for (auto& k : tmp->at(tag)) {
                            if (k.contains(arg) && k.at(arg) == arg_value) {
                                tmp = &k;
                                break;
                            }
                        }
                    }
                    else {
                        tmp->operator[](tag) = nlohmann::json();
                    }
                    if (m == tmp) {
                        tmp->at(tag).push_back(nlohmann::json({{match[2].str(), match[3].str()}}));
                        tmp = &tmp->at(tag).back();
                    }
                }
                else {
                    if (tmp->contains(tag)) {
                        tmp = &tmp->at(tag);
                    }
                    else {
                        tmp->operator[](tag) = nlohmann::json();
                        tmp = &tmp->at(tag);
                    }
                }
            }
        }
        *tmp = decltype(value)::value_type(value);
    }
    return std::move(j);
}

MetricsCollector::counter_t& MetricsCollector::make_counter(const std::string& name) {
    return _this.counter_map[name];
}

MetricsCollector::counter_t* MetricsCollector::make_counter_ptr(const std::string& name) {
    return &_this.counter_map[name];
}
