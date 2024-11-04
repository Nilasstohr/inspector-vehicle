#include "rclcpp/rclcpp.hpp"
#include "Utilities/SerialInterface.h"
#include "Configurations.h"
#include "ControllerNode.h"
#include "test_header.h"

using Eigen::MatrixXd;
using Eigen::Vector3d;

using std::placeholders::_1;

#define ROS_INFO RCUTILS_LOG_INFO
#define RAD2DEG(x) ((x)*180./M_PI)

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ControllerNode>(new SerialInterface(CONFIG_ROBOT_DRIVER_DEVICE_NAME));
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
