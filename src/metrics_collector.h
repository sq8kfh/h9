/*
 * Created by crowx on 18/09/2023.
 *
 */

#pragma once

#include <atomic>
#include <ctime>
#include <map>
#include <nlohmann/json.hpp>

class MetricsCollector {
  public:
    using counter_t = std::atomic_uint64_t;

  private:
    static MetricsCollector _this;

    const time_t start_time;

    std::map<std::string, counter_t> counter_map;
    std::atomic_uint64_t test_counter;
    MetricsCollector();
  public:
    static nlohmann::json metrics_to_json();
    static counter_t& make_counter(const std::string& name);
    static counter_t* make_counter_ptr(const std::string& name);
};
