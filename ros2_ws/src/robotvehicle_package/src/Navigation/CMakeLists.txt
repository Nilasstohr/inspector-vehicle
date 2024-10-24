add_library(
    NavigationLib
    Navigator.cpp
    NavigationPath.cpp
    NavigationPoint.cpp)
ament_target_dependencies(NavigationLib rclcpp sensor_msgs)
target_link_libraries (NavigationLib Eigen3::Eigen)