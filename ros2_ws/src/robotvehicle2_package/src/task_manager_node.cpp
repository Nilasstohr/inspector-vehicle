#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "TaskManagerNode.h"

#define ROS_INFO RCUTILS_LOG_INFO

int main(const int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    const auto node = std::make_shared<TaskManagerNode>();
    spin(node);
    rclcpp::shutdown();
    return 0;
}


