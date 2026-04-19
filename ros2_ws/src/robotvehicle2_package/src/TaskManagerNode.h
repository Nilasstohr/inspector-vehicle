//
// Created by robot1 on 1/24/26.
//

#ifndef TaskManagerNode_H
#define TaskManagerNode_H

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

#include "BehaviourTree/NavState.h"
#include <behaviortree_cpp_v3/bt_factory.h>

#include <memory>
#include <vector>

class TaskManagerNode final : public rclcpp::Node {
public:
    using NavigateToPose = nav2_msgs::action::NavigateToPose;
    using GoalHandle     = rclcpp_action::ClientGoalHandle<NavigateToPose>;

    explicit TaskManagerNode();

    /// Queue a goal for the BT to process on the next tick.
    void queue_goal(double x, double y, double yaw_rad = 0.0);

private:
    // --- action client ---
    rclcpp_action::Client<NavigateToPose>::SharedPtr nav_client_;
    rclcpp_action::Client<NavigateToPose>::SendGoalOptions send_goal_options_;

    rclcpp::TimerBase::SharedPtr control_timer_;

    // --- behaviour tree ---
    std::shared_ptr<NavState>  nav_state_;
    BT::BehaviorTreeFactory    bt_factory_;
    BT::Tree                   bt_tree_;

    void build_behaviour_tree();
    void bt_tick();

    /// Low-level goal sender used by NavigateNode. Returns false if server unavailable.
    bool send_goal(double x, double y, double yaw_rad) const;

    // --- action callbacks ---
    void goal_response_callback(const GoalHandle::SharedPtr & goal_handle) const;
    void feedback_callback(const GoalHandle::SharedPtr &,
                           const std::shared_ptr<const NavigateToPose::Feedback> & feedback);
    void result_callback(const GoalHandle::WrappedResult & result) const;
};


#endif //TaskManagerNode_H
