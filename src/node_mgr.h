/*
 * H9 project
 *
 * Created by crowx on 2023-09-19.
 *
 */

#pragma once

#include "bus.h"
#include "frameobserver.h"
#include "framesubject.h"
#include "node.h"

class NodeMgr: public FrameSubject {
    // TODO: nadpisac notify_frame_observer - żeby zmniejszyc zlozonosc, break po pierwszym match
  private:
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
  protected:
    virtual void on_frame_recv(const ExtH9Frame& frame) noexcept;
    Bus* bus;
  public:
    NodeMgr(Bus* bus);
    Node get_node(std::uint16_t node_id);
    void response_timeout_duration(int response_timeout_duration);
    int response_timeout_duration();
};
