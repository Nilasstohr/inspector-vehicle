add_library(TestLib
        ../../test/TestUtilities/SerialInterfaceStub.cpp
)
ament_target_dependencies(TestLib rclcpp sensor_msgs)
target_link_libraries (TestLib Eigen3::Eigen)
