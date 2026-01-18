#!/bin/bash
source /opt/ros/humble/setup.bash
ros2 run nav2_map_server map_saver_cli -f /home/robot1/devrepo/inspector-vehicle/ros2_ws/my_map --ros-args -p save_map_timeout:=29.0
