add_library(PathPlanningLib
        AStar.cpp
        GridMap.cpp
        LinePoints.cpp
        ObstacleDetection.cpp
        PathPoint.cpp
)
target_link_libraries (PathPlanningLib Eigen3::Eigen)
ament_target_dependencies(PathPlanningLib rclcpp sensor_msgs)