#include <gtest/gtest.h>
#include "rclcpp/rclcpp.hpp"
#include "Configurations.h"
#include "Localization/Incremental.h"
#include "TestUtilities/RecordHandler.h"
#include "Host/DriverInterface.h"
#include "TestUtilities/SerialInterfaceStub.h"
#include "Sensor/SensorData.h"
#include "Configurations.h"
int argcTest;
char **argvTest;


TEST(line_least_square_test,test_1)
{
    Line line;
    line.addRecPoint(199,0);
    line.addRecPoint(198.9966,1.1541);
    line.addRecPoint(198.786624,2.3060);
    line.addRecPoint(198.7696,3.4788);
    line.addRecPoint(198.9459,4.6362);
    line.updateSlopeFormLeastSquare();
    double toleranceM = 0.1;
    double toleranceB = 2;
    double expectedM = -7.55;
    double expectedB = 1504.94;
    if(line.getM() > expectedM + toleranceM ||  line.getM() < expectedM - toleranceM) {
        EXPECT_TRUE(false) << "Expected slope to be " << expectedM << " +- " << toleranceM
        << " but got " << line.getM() << std::endl;
    }
    if(line.getB() > expectedB + toleranceB ||  line.getB() < expectedB - toleranceB) {
        EXPECT_TRUE(false) << "Expected intercept to be " << expectedB << " +- " << toleranceB
        << " but got " << line.getB() << std::endl;
    }
}

TEST(line_detection_integration_test,test_2)
{
    int MAX_LINES_EXPECTED = 10;
    auto *serial_interface = new SerialInterface(CONFIG_ROBOT_DRIVER_DEVICE_NAME);
    auto *driver_interface = new DriverInterface(serial_interface);
    auto * sensor_data = new SensorData(driver_interface);
    auto *recordHandler = new RecordHandler();
    auto * incremental = new Incremental(CONFIG_ESP);
    while(recordHandler->hasRecordsToProcess()) {
        recordHandler->update(sensor_data);
        incremental->update(sensor_data->getScanPolarForm(),
            sensor_data->getScanPolarForm()->size());
        int linesDetected = incremental->size();
        cout<<"linesDetected: "<<linesDetected<<endl;
        if(linesDetected>MAX_LINES_EXPECTED) {
            EXPECT_TRUE(false) << "expected less that " << MAX_LINES_EXPECTED << " lines but got " << linesDetected;
        }
    }
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}