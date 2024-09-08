#!/bin/bash
# example of using arguments to a script
source /opt/ros/foxy/setup.bash
source install/setup.bash
sudo chmod 666 /dev/ttyUSB0
ros2 run rplidar_ros rplidarNode node
#echo $(pwd)   

