add_library(UtilitiesLib
        AwaitTimer.cpp
        SerialInterface.cpp
        UsbSerial.cpp
        String.cpp
        MathConversions.cpp
        MatrixStackBase.cpp
        MatrixHelper.cpp
)
ament_target_dependencies(UtilitiesLib rclcpp)
target_link_libraries (UtilitiesLib Eigen3::Eigen)
