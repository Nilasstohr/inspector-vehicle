add_library(PathPlanningLib
        AStar.cpp
        GridMap.cpp
        LinePoints.cpp
        ObstacleDetection.cpp
        PathPoint.cpp
        CirclePoints.cpp
        PointBase.cpp
        AStar2.cpp
)
target_link_libraries (PathPlanningLib Eigen3::Eigen UtilitiesLib)
ament_target_dependencies(PathPlanningLib rclcpp sensor_msgs)