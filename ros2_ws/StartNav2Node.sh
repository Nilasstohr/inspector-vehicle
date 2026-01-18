#!/bin/bash
source /opt/ros/humble/setup.bash
ros2 launch nav2_bringup navigation_launch.py params_file:=/home/robot1/devrepo/inspector-vehicle/ros2_ws/nav2_config.yaml

