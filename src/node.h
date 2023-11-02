/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_NODE_H
#define H9_NODE_H

#include "config.h"

#include <exception>
#include <map>
#include <mutex>
#include <set>
#include <variant>
#include <vector>

#include "node_desc_loader.h"
#include "raw_node.h"

class NodeDevMgr;
class TCPClientThread;

class Node: protected RawNode {
  public:
    using RegisterDsc = NodeDescLoader::RegisterDesc;
    using regvalue_t = std::variant<std::string, std::int64_t, std::vector<std::uint8_t>>;

    class NodeStateObserver {
      public:
        virtual void update_dev_state(std::uint16_t node_id, const ExtH9Frame& frame) = 0;
    };

  private:
    std::shared_ptr<spdlog::logger> logger;
    static NodeDescLoader nodedescloader;

    const std::uint16_t _device_type;
    const std::uint64_t _device_version;
    std::string _device_name;
    std::string _device_description;
    const std::time_t _created_time;
    std::time_t _last_seen_time;

    std::vector<NodeStateObserver*> node_state_observers;
    std::map<std::uint8_t, RegisterDsc> register_map;

    friend class NodeDevMgr;
    void update_node_state(const ExtH9Frame& frame);
    void attach_node_state_observer(Node::NodeStateObserver* obs);
    void detach_node_state_observer(Node::NodeStateObserver* obs);
    void update_node_last_seen_time() noexcept;

  protected:
    ////    void notify_event_observer(std::string event_name, GenericMsg msg) noexcept;
    Node(NodeDevMgr* node_mgr, Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept;
  public:
    ~Node();
    std::vector<RegisterDsc> get_registers_list() noexcept;

    [[nodiscard]] std::uint16_t device_type() const noexcept;
    [[nodiscard]] std::uint64_t device_version() const noexcept;
    [[nodiscard]] std::uint16_t device_version_major() const noexcept;
    [[nodiscard]] std::uint16_t device_version_minor() const noexcept;
    [[nodiscard]] std::uint16_t device_version_patch() const noexcept;
    [[nodiscard]] std::string device_name() const noexcept;
    [[nodiscard]] std::time_t device_created_time() const noexcept;
    [[nodiscard]] std::time_t device_last_seen_time() const noexcept;
    [[nodiscard]] std::string device_description() const noexcept;

    void node_reset();
    regvalue_t set_register(std::uint8_t reg, regvalue_t value);
    regvalue_t get_register(std::uint8_t reg);
    regvalue_t set_register_bit(std::uint8_t reg, std::uint8_t bit_num);
    regvalue_t clear_register_bit(std::uint8_t reg, std::uint8_t bit_num);
    regvalue_t toggle_register_bit(std::uint8_t reg, std::uint8_t bit_num);
};

#endif // H9_NODE_H
