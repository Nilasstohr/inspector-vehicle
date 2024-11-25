#!/bin/bash
source /opt/ros/humble/setup.bash
colcon build --packages-select robotvehicle_package sllidar_ros2
source install/setup.bash
