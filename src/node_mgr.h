/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include <atomic>
#include <map>
#include <queue>
#include <shared_mutex>
#include <spdlog/spdlog.h>
#include <thread>

#include "bus.h"
#include "frameobserver.h"
#include "framesubject.h"
#include "node.h"
#include "raw_node.h"

class TCPClientThread;

class NodeMgr: public FrameSubject {
  private:
    std::shared_ptr<spdlog::logger> logger;
    Bus* bus;

    // TODO: nadpisac notify_frame_observer - żeby zmniejszyc zlozonosc, break po pierwszym match
    class NodeMgrFrameObs: public FrameObserver {
        NodeMgr* node_mgr;

      public:
        NodeMgrFrameObs(Bus* bus, NodeMgr* node_mgr):
            FrameObserver(bus, H9FrameComparator()),
            node_mgr(node_mgr) {}

        void on_frame_recv(const ExtH9Frame& frame) {
            node_mgr->on_frame_recv(frame);
        }
    };

    NodeMgrFrameObs frame_obs;
    int _response_timeout_duration;

    virtual void on_frame_recv(const ExtH9Frame& frame) noexcept;

    std::shared_mutex nodes_map_mtx;

    std::map<std::uint16_t, Node*> nodes_map;

    std::mutex frame_queue_mtx;
    std::condition_variable frame_queue_cv; // TODO: counting_semaphore (C++20)?
    std::queue<ExtH9Frame> frame_queue;

    std::atomic_bool nodes_update_thread_run;
    std::thread nodes_update_thread_desc;
    void nodes_update_thread();

    Node* build_node(std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept;
    void add_node(std::uint16_t node_id, std::uint16_t node_type, std::uint64_t node_version) noexcept;

  public:
    struct NodeDsc {
        std::uint16_t id;
        std::uint16_t type;
        std::uint16_t version_major;
        std::uint16_t version_minor;
        std::uint16_t version_patch;
        std::string name;
    };

    struct NodeInfo: public NodeDsc {
        std::time_t created_time;
        std::time_t last_seen_time;
        std::string description;
    };

    explicit NodeMgr(Bus* bus);
    NodeMgr(const NodeMgr& a) = delete;
    ~NodeMgr();
    void load_devices_description(const std::string& nodes_description_filename);

    void response_timeout_duration(int response_timeout_duration);
    int response_timeout_duration();

    int discover() noexcept;

    int active_devices_count() noexcept;
    bool is_node_exist(std::uint16_t node_id) noexcept;
    std::vector<NodeMgr::NodeDsc> get_nodes_list() noexcept;

    //    int attach_event_observer(TCPClientThread* observer, const std::string& event_name, std::uint16_t dev_id) noexcept;
    //    int detach_event_observer(TCPClientThread* observer, const std::string& event_name, std::uint16_t dev_id) noexcept;
    //    std::vector<std::string> get_events_list(std::uint16_t dev_id) noexcept;
    //
    //    std::vector<std::string> get_device_specific_methods(std::uint16_t dev_id) noexcept;
    // H9Value execute_device_specific_method(std::uint16_t dev_id, const std::string& method_name, const H9Tuple& tuple);

    std::vector<Node::RegisterDsc> get_registers_list(std::uint16_t node_id) noexcept;

    int get_node_info(std::uint16_t dev_id, NodeInfo& device_info);
    void node_reset(std::uint16_t node_id);
    Node::regvalue_t set_register(std::uint16_t node_id, std::uint8_t reg, Node::regvalue_t value);
    Node::regvalue_t get_register(std::uint16_t node_id, std::uint8_t reg);
    Node::regvalue_t set_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num);
    Node::regvalue_t clear_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num);
    Node::regvalue_t toggle_register_bit(std::uint16_t node_id, std::uint8_t reg, std::uint8_t bit_num);
};
