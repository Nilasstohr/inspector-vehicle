#!/bin/bash
source /opt/ros/humble/setup.bash
colcon test --packages-select robotvehicle_package 
colcon test-result --all --verbose
