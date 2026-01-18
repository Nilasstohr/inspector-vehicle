#!/bin/bash
source /opt/ros/humble/setup.bash
ros2 launch slam_toolbox online_async_launch.py \
  slam_params_file:=/home/robot1/devrepo/inspector-vehicle/ros2_ws/slamtec_config.yaml \
  mode:=lifelong \
  map_file_name:=/home/robot1/devrepo/inspector-vehicle/ros2_ws/my_map
