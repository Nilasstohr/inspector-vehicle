#!/bin/bash
# example of using arguments to a script
source /opt/ros/humble/setup.bash
source install/setup.bash
sudo chmod 666 /dev/ttyUSB0
ros2 launch sllidar_ros2 sllidar_a2m8_launch.py
#echo $(pwd)   

