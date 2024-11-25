#!/bin/bash
source /opt/ros/humble/setup.bash
source install/setup.bash
sudo chmod 666 /dev/ttyUSB0
#ros2 run robotvehicle_package controller_node
ros2 launch robotvehicle_package launch_vehicle_robot.py
