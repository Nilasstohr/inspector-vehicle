#!/bin/bash
source /opt/ros/humble/setup.bash
ros2 run tf2_ros static_transform_publisher 0 0 0 3.14159 0 0 base_link laser
