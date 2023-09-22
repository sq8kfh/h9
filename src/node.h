/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020-2023 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_NODE_H
#define H9_NODE_H

#include "config.h"

#include <chrono>
#include <future>
#include <queue>
#include <set>
#include <spdlog/spdlog.h>
#include <tuple>

#include "frameobserver.h"

class NodeMgr;
class Bus;

class Node: public FrameObserver {
  private:
    class FramePromise {
        Node* const node;
        bool comparator_has_seqnum;
        H9FrameComparator comparator;
        std::queue<ExtH9Frame> frame_storage;

        std::promise<ExtH9Frame> promise;

      public:
        FramePromise(Node* node, H9FrameComparator comparator):
            node(node),
            comparator_has_seqnum(false),
            comparator(comparator) {}

        void on_frame(const ExtH9Frame& frame) {
            if (comparator == frame) {
                if (comparator_has_seqnum) {
                    promise.set_value(frame);
                }
                else {
                    frame_storage.push(frame);
                }
            }
        }

        void set_comparator_seqnum(uint8_t seqnum) {
            node->frame_promise_set_mtx.lock();
            comparator.set_seqnum(seqnum);
            comparator_has_seqnum = true;
            while (!frame_storage.empty()) {
                on_frame(frame_storage.front());
                frame_storage.pop();
            }
            node->frame_promise_set_mtx.unlock();
        }

        std::future<ExtH9Frame> get_future() {
            return promise.get_future();
        }
    };

    NodeMgr* const node_mgr;
    Bus* const bus;

    std::mutex frame_promise_set_mtx;
    std::set<FramePromise*> frame_promise_set;

    void on_frame_recv(const ExtH9Frame& frame) noexcept;
    FramePromise* create_frame_promise(H9FrameComparator comparator);
    void destroy_frame_promise(FramePromise* frame_promise);

    ssize_t bit_operation(const std::string& origin, H9frame::Type type, std::uint8_t reg, std::uint8_t bit, std::size_t length = 0, std::uint8_t* reg_after_set = nullptr) noexcept;

  protected:
    const std::uint16_t _node_id;

  public:
    constexpr static std::uint8_t REG_NODE_TYPE = 1;
    constexpr static std::uint8_t REG_NODE_VERSION = 2;
    constexpr static std::uint8_t REG_NODE_METADATA = 3;
    constexpr static std::uint8_t REG_NODE_ID = 4;
    constexpr static std::uint8_t REG_NODE_MCU_TYPE = 5;

    constexpr static ssize_t TIMEOUT_ERROR = -1000;
    constexpr static ssize_t MALFORMED_FRAME_ERROR = -1001;

    Node(NodeMgr* node_mgr, Bus* bus, std::uint16_t node_id) noexcept;
    ~Node() noexcept;

    std::uint16_t node_id() noexcept;

    ssize_t reset(const std::string& origin) noexcept;

    int32_t get_node_type(const std::string& origin) noexcept;
    int64_t get_node_version(const std::string& origin, std::uint16_t* major = nullptr, std::uint16_t* minor = nullptr, std::uint16_t* patch = nullptr) noexcept;
    int32_t get_mcu_type(const std::string& origin) noexcept;

    void firmware_update(const std::string& origin, void (*progress_callback)(int percentage));

    ssize_t set_bit(const std::string& origin, std::uint8_t reg, std::uint8_t bit, std::size_t length = 0, std::uint8_t* reg_after_set = nullptr) noexcept;
    ssize_t clear_bit(const std::string& origin, std::uint8_t reg, std::uint8_t bit, std::size_t length = 0, std::uint8_t* reg_after_set = nullptr) noexcept;
    ssize_t toggle_bit(const std::string& origin, std::uint8_t reg, std::uint8_t bit, std::size_t length = 0, std::uint8_t* reg_after_set = nullptr) noexcept;

    ssize_t set_reg(const std::string& origin, std::uint8_t reg, std::size_t length, std::uint8_t* reg_val, std::uint8_t* reg_after_set = nullptr) noexcept;
    ssize_t set_reg(const std::string& origin, std::uint8_t reg, std::uint8_t reg_val, std::uint8_t* reg_after_set = nullptr) noexcept;
    ssize_t set_reg(const std::string& origin, std::uint8_t reg, std::uint16_t reg_val, std::uint16_t* reg_after_set = nullptr) noexcept;
    ssize_t set_reg(const std::string& origin, std::uint8_t reg, std::uint32_t reg_val, std::uint32_t* reg_after_set = nullptr) noexcept;

    /// Read registry from the node
    /// @param[in] origin client idstring
    /// @param[in] reg number to the read
    /// @param[in] nbyte size of reg_val
    /// @param[out] reg_val
    /// @retval >=0  Number of read byte
    /// @retval TIMEOUT_ERROR on timeout
    /// @retval -H9Frame::Error on node error (received ERROR frame)
    ssize_t get_reg(const std::string& origin, std::uint8_t reg, std::size_t length, std::uint8_t* reg_val) noexcept;
    ssize_t get_reg(const std::string& origin, std::uint8_t reg, std::uint8_t* reg_val) noexcept;
    ssize_t get_reg(const std::string& origin, std::uint8_t reg, std::uint16_t* reg_val) noexcept;
    ssize_t get_reg(const std::string& origin, std::uint8_t reg, std::uint32_t* reg_val) noexcept;
};

#endif // H9_NODE_H
