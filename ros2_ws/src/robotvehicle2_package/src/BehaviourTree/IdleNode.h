#pragma once
#include <behaviortree_cpp_v3/action_node.h>
#include "NavState.h"
#include "rclcpp/rclcpp.hpp"
#include <memory>

/// BT leaf: waits until a goal has been queued in NavState.
/// Returns SUCCESS as soon as has_goal == true, RUNNING otherwise.
class IdleNode : public BT::StatefulActionNode {
public:
    IdleNode(const std::string & name, const BT::NodeConfiguration & config, std::shared_ptr<NavState> state,  const rclcpp::Logger  &logger):
    StatefulActionNode(name, config), state_(std::move(state)), logger_(logger) {}

    static BT::PortsList providedPorts() { return {}; }

    BT::NodeStatus onStart() override { return onRunning(); }

    BT::NodeStatus onRunning() override {
        if (state_->has_goal) {
            RCLCPP_INFO(logger_, "[BT] IdleNode → goal queued, transitioning to Navigate");
            return BT::NodeStatus::SUCCESS;
        }
        RCLCPP_DEBUG(logger_, "[BT] IdleNode → waiting for a goal…");
        return BT::NodeStatus::RUNNING;
    }

    void onHalted() override {}

private:
    std::shared_ptr<NavState> state_;
    rclcpp::Logger             logger_;
};
