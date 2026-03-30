add_library(SensorLib
    PointPolarForm.cpp
    PointRectForm.cpp
)
ament_target_dependencies(SensorLib rclcpp sensor_msgs)
