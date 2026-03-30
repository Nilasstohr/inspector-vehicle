#!/bin/bash
set -e
source /opt/ros/humble/setup.bash

# Clone all source dependencies declared in deps.repos (e.g. sllidar_ros2)
vcs import src < src/deps.repos

# Refresh apt and install all system dependencies declared in each package.xml
sudo apt-get update
rosdep update
rosdep install --from-paths src --ignore-src -r -y

colcon build --packages-select robotvehicle_package sllidar_ros2

source install/setup.bash
