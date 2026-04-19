#pragma once
#include <behaviortree_cpp_v3/action_node.h>
#include "NavState.h"
#include "rclcpp/rclcpp.hpp"
#include <memory>

/// BT leaf: runs when navigation has succeeded.
/// Logs the achievement and returns SUCCESS immediately.
class GoalReachedNode : public BT::SyncActionNode {
public:
    GoalReachedNode(const std::string & name, const BT::NodeConfiguration & config,
                    std::shared_ptr<NavState> state, rclcpp::Logger logger)
        : BT::SyncActionNode(name, config)
        , state_(std::move(state)), logger_(logger) {}

    static BT::PortsList providedPorts() { return {}; }

    BT::NodeStatus tick() override {
        RCLCPP_INFO(logger_,
            "[BT] GoalReachedNode → goal (%.2f, %.2f) reached! Robot is idle.",
            state_->goal_x, state_->goal_y);
        return BT::NodeStatus::SUCCESS;
    }

private:
    std::shared_ptr<NavState> state_;
    rclcpp::Logger             logger_;
};
