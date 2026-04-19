#pragma once
#include <behaviortree_cpp_v3/action_node.h>
#include "NavState.h"
#include "rclcpp/rclcpp.hpp"
#include <functional>
#include <memory>

/// BT leaf: sends the queued goal to Nav2 and waits for the result.
///
///  onStart()   – fires the goal once → RUNNING
///  onRunning() – polls NavState::phase every tick
///    NAVIGATING → RUNNING  |  SUCCEEDED → SUCCESS  |  FAILED → FAILURE
class NavigateNode : public BT::StatefulActionNode {
public:
    using SendGoalFn = std::function<bool(double, double, double)>;

    NavigateNode(const std::string & name, const BT::NodeConfiguration & config,std::shared_ptr<NavState>
        state, SendGoalFn send_goal_fn,const rclcpp::Logger & logger):
          StatefulActionNode(name, config)
        , state_(std::move(state))
        , send_goal_fn_(std::move(send_goal_fn))
        , logger_(logger) {}

    static BT::PortsList providedPorts() { return {}; }

    /// Called exactly once when the node first becomes active.
    BT::NodeStatus onStart() override {
        double x{}, y{}, yaw{};
        if (!state_->consume_goal(x, y, yaw)) {
            RCLCPP_WARN(logger_, "[BT] NavigateNode → no goal to consume");
            return BT::NodeStatus::FAILURE;
        }
        RCLCPP_INFO(logger_, "[BT] NavigateNode → sending goal (%.2f, %.2f, %.2f)", x, y, yaw);
        if (!send_goal_fn_(x, y, yaw)) {
            RCLCPP_ERROR(logger_, "[BT] NavigateNode → send_goal failed (server unavailable?)");
            return BT::NodeStatus::FAILURE;
        }
        return BT::NodeStatus::RUNNING;
    }

    /// Called on every subsequent tick while RUNNING.
    BT::NodeStatus onRunning() override {
        switch (state_->phase) {
            case NavState::Phase::NAVIGATING:
                return BT::NodeStatus::RUNNING;
            case NavState::Phase::SUCCEEDED:
                RCLCPP_INFO(logger_, "[BT] NavigateNode → navigation SUCCEEDED");
                return BT::NodeStatus::SUCCESS;
            case NavState::Phase::FAILED:
                RCLCPP_ERROR(logger_, "[BT] NavigateNode → navigation FAILED");
                return BT::NodeStatus::FAILURE;
            default:
                return BT::NodeStatus::RUNNING;
        }
    }

    /// Called when a parent node halts this node mid-flight.
    void onHalted() override {
        state_->set_phase(NavState::Phase::IDLE);
        RCLCPP_WARN(logger_, "[BT] NavigateNode → halted");
    }

private:
    std::shared_ptr<NavState> state_;
    SendGoalFn                send_goal_fn_;
    rclcpp::Logger            logger_;
};

