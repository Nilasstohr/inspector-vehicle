#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include "Host/DriverInterface.h"
#include "Utilities/SerialInterface.h"
#include "Configurations.h"

DriverInterface * driver_interface;
double leftWheelPosition;
double rightWheelPosition;

void updatePosition(double &leftWheelPosition,double &rightWheelPosition) {
    driver_interface->getWheelsTraveled(leftWheelPosition,rightWheelPosition    );
}

TEST(host_system_test, position_zero_after_reset)
{
    driver_interface->reset();
    updatePosition(leftWheelPosition,rightWheelPosition);
    driver_interface->stop();
    if(leftWheelPosition<0 || rightWheelPosition>0) {
        EXPECT_TRUE(false) << "position was not initialized correctly";
    }
}

TEST(host_system_test,confirm_position_increase) {
    driver_interface->reset();
    driver_interface->setAngularVelocity(2,2 );
    sleep(2 );
    updatePosition(leftWheelPosition,rightWheelPosition);
    driver_interface->stop();
    if(leftWheelPosition<=0 && rightWheelPosition<=0) {
        EXPECT_TRUE(false) << "motors does not seem to be running";
    }
}

TEST(host_system_test,stress_test) {
    driver_interface->reset();
    while(1) {
        updatePosition(leftWheelPosition,rightWheelPosition);
        driver_interface->setAngularVelocity(0,0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    driver_interface->setAngularVelocity(2,2 );
    sleep(2 );
    updatePosition(leftWheelPosition,rightWheelPosition);
    driver_interface->stop();
    if(leftWheelPosition<=0 && rightWheelPosition<=0) {
        EXPECT_TRUE(false) << "motors does not seem to be running";
    }
}


int main(int argc, char ** argv)
{
    auto serial_interface =  SerialInterface(CONFIG_ROBOT_DRIVER_DEVICE_NAME);
    driver_interface = new DriverInterface(serial_interface);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}