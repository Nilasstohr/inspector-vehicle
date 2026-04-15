#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "Configurations.h"
#include "TestUtilities/SerialInterfaceStub.h"
#include "TestUtilities/RecordHandler.h"
#include "ControllerNodeStub.h"
#include "ExceptionIncorrectEndPosition.h"

int argcTest;
char **argvTest;

TEST(controller_node_integration_test,test_1)
{
    rclcpp::init(argcTest,argvTest);
    auto startTime = std::chrono::steady_clock::now();
    try {
        SerialInterfaceStub serialInterfaceStub(CONFIG_ROBOT_DRIVER_DEVICE_NAME);
        auto node = std::make_shared<ControllerNodeStub>(serialInterfaceStub);
        rclcpp::spin(node);
        rclcpp::shutdown();
    }catch (runtime_error *e) {
        EXPECT_TRUE(false) << e->what();
    }
    auto endTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(endTime - startTime).count();
    std::cout << "[TIMING] Test duration: " << elapsed << " s" << std::endl;
}

int main(int argc, char ** argv)
{
    argcTest = 1;
    argvTest = argv;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}