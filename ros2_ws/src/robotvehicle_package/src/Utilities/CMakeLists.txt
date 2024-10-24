add_library(UtilitiesLib
    MathConversions.cpp
    AwaitTimer.cpp
    MatrixStackBase.cpp
    MatrixHelper.cpp
    Pose.cpp
    SerialInterface.cpp
    Transformations.cpp
    UsbSerial.cpp
    Verify2DArea.cpp)
ament_target_dependencies(UtilitiesLib rclcpp)
target_link_libraries (UtilitiesLib Eigen3::Eigen)
