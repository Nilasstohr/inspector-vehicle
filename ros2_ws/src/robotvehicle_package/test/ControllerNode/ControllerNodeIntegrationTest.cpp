#include <gtest/gtest.h>
#include "rclcpp/rclcpp.hpp"
#include "Configurations.h"
#include "TestUtilities/SerialInterfaceStub.h"
#include "ControllerNodeStub.h"

int argcTest;
char **argvTest;

TEST(controller_node_integration_test,test_1)
{
    rclcpp::init(argcTest,argvTest);
    auto node = std::make_shared<ControllerNodeStub>(new SerialInterfaceStub(CONFIG_ROBOT_DRIVER_DEVICE_NAME));
    rclcpp::spin(node);
    rclcpp::shutdown();
}

int main(int argc, char ** argv)
{
    argcTest = 1;
    argvTest = argv;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}