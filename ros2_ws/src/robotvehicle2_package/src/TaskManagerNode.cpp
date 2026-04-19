//
// Created by robot1 on 11/1/24.
//

#include "TaskManagerNode.h"
#include "BehaviourTree/IdleNode.h"
#include "BehaviourTree/NavigateNode.h"
#include "BehaviourTree/GoalReachedNode.h"
#include "tf2/LinearMath/Quaternion.h"
#include <behaviortree_cpp_v3/bt_factory.h>
#include <functional>

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────
TaskManagerNode::TaskManagerNode() : Node("task_manager_node")
{
    // Create the action client targeting the Nav2 navigation server
    nav_client_ = rclcpp_action::create_client<NavigateToPose>(this, "navigate_to_pose");

    // Wire up callbacks once – reused on every send_goal() call
    send_goal_options_.goal_response_callback =
        std::bind(&TaskManagerNode::goal_response_callback, this, std::placeholders::_1);
    send_goal_options_.feedback_callback =
        std::bind(&TaskManagerNode::feedback_callback, this,
                  std::placeholders::_1, std::placeholders::_2);
    send_goal_options_.result_callback =
        std::bind(&TaskManagerNode::result_callback, this, std::placeholders::_1);

    // Build the shared state and behaviour tree
    nav_state_ = std::make_shared<NavState>();
    build_behaviour_tree();

    // Tick the BT every 200 ms
    control_timer_ = this->create_wall_timer(
        std::chrono::milliseconds(200),
        std::bind(&TaskManagerNode::bt_tick, this));

    RCLCPP_INFO(get_logger(), "TaskManagerNode started – waiting for a goal…");

    // ── Demo: queue a first goal right away ──────────────────────────────
    //queue_goal(0.6, 0.6, 0.0);
}

// ──────────────────────────────────────────────
// BT wiring
// ──────────────────────────────────────────────
void TaskManagerNode::build_behaviour_tree()
{
    // ── Register custom node types with their extra constructor args ──────
    bt_factory_.registerBuilder<IdleNode>(
        "IdleNode",
        [this](const std::string & name, const BT::NodeConfiguration & cfg) {
            return std::make_unique<IdleNode>(name, cfg, nav_state_, get_logger());
        });

    bt_factory_.registerBuilder<NavigateNode>(
        "NavigateNode",
        [this](const std::string & name, const BT::NodeConfiguration & cfg) {
            return std::make_unique<NavigateNode>(
                name, cfg, nav_state_,
                std::bind(&TaskManagerNode::send_goal, this,
                          std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                get_logger());
        });

    bt_factory_.registerBuilder<GoalReachedNode>(
        "GoalReachedNode",
        [this](const std::string & name, const BT::NodeConfiguration & cfg) {
            return std::make_unique<GoalReachedNode>(name, cfg, nav_state_, get_logger());
        });

    // ── Describe the mission as an XML tree ───────────────────────────────
    // RetryUntilSuccessful(num_attempts=3) retries NavigateNode up to 3 times
    // before letting the failure propagate. Add more decorators here later.
    static const char * xml_tree = R"(
        <root main_tree_to_execute="Mission">
          <BehaviorTree ID="Mission">
            <Sequence>
              <IdleNode     name="WaitForGoal"/>
              <RetryUntilSuccessful num_attempts="3">
                <NavigateNode name="Navigate"/>
              </RetryUntilSuccessful>
              <GoalReachedNode name="GoalReached"/>
            </Sequence>
          </BehaviorTree>
        </root>
    )";

    bt_tree_ = bt_factory_.createTreeFromText(xml_tree);
}

void TaskManagerNode::bt_tick() {
    bt_tree_.tickRoot();
}

// ──────────────────────────────────────────────
// Public API
// ──────────────────────────────────────────────
void TaskManagerNode::queue_goal(const double x, const double y, const double yaw_rad) {
    RCLCPP_INFO(get_logger(), "Goal queued: x=%.2f  y=%.2f  yaw=%.2f", x, y, yaw_rad);
    nav_state_->queue_goal(x, y, yaw_rad);
    bt_tree_.haltTree();   // restart the sequence from Idle
}

bool TaskManagerNode::send_goal(const double x, const double y, const double yaw_rad) const {
    if (!nav_client_->wait_for_action_server(std::chrono::seconds(5))) {
        RCLCPP_ERROR(get_logger(), "NavigateToPose action server not available!");
        return false;
    }

    auto goal_msg = NavigateToPose::Goal();
    goal_msg.pose.header.frame_id = "map";
    goal_msg.pose.header.stamp    = now();

    goal_msg.pose.pose.position.x = x;
    goal_msg.pose.pose.position.y = y;
    goal_msg.pose.pose.position.z = 0.0;

    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, yaw_rad);
    goal_msg.pose.pose.orientation.x = q.x();
    goal_msg.pose.pose.orientation.y = q.y();
    goal_msg.pose.pose.orientation.z = q.z();
    goal_msg.pose.pose.orientation.w = q.w();

    RCLCPP_INFO(get_logger(), "Sending goal  x=%.2f  y=%.2f  yaw=%.2f", x, y, yaw_rad);
    nav_client_->async_send_goal(goal_msg, send_goal_options_);
    return true;
}

// ──────────────────────────────────────────────
// Action callbacks  (write into NavState so the BT can read on next tick)
// ──────────────────────────────────────────────
void TaskManagerNode::goal_response_callback(const GoalHandle::SharedPtr & goal_handle) const
{
    if (!goal_handle) {
        RCLCPP_ERROR(get_logger(), "Goal was rejected by the Nav2 server!");
        nav_state_->set_phase(NavState::Phase::FAILED);  // notify the BT
    } else {
        RCLCPP_INFO(get_logger(), "Goal accepted – robot is navigating…");
    }
}

void TaskManagerNode::feedback_callback(
    const GoalHandle::SharedPtr & /*goal_handle*/,
    const std::shared_ptr<const NavigateToPose::Feedback> & feedback)
{
    auto & pose = feedback->current_pose.pose;
    RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 300 /*ms*/,
        "Feedback – pose: x=%.2f  y=%.2f  remaining=%.2f m",
        pose.position.x, pose.position.y,
        feedback->distance_remaining);
}

void TaskManagerNode::result_callback(const GoalHandle::WrappedResult & result) const {
    switch (result.code) {
        case rclcpp_action::ResultCode::SUCCEEDED:
            RCLCPP_INFO(get_logger(), "Goal reached successfully!");
            nav_state_->set_phase(NavState::Phase::SUCCEEDED);
            break;
        case rclcpp_action::ResultCode::ABORTED:
            RCLCPP_ERROR(get_logger(), "Goal was aborted by the server.");
            nav_state_->set_phase(NavState::Phase::FAILED);
            break;
        case rclcpp_action::ResultCode::CANCELED:
            RCLCPP_WARN(get_logger(), "Goal was cancelled.");
            nav_state_->set_phase(NavState::Phase::FAILED);
            break;
        default:
            RCLCPP_ERROR(get_logger(), "Unknown result code.");
            nav_state_->set_phase(NavState::Phase::FAILED);
            break;
    }
}
