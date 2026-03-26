#!/bin/bash

# Load ROS2 environment variables and paths (needed for any ros2 command to work)
source /opt/ros/humble/setup.bash

# Load our own built packages so ros2 can find robotvehicle_package
source install/setup.bash

# The lidar is on /dev/ttyUSB0. By default only root can read/write it.
# chmod 666 gives read/write to all users so our ROS node can access it.
if [ -e /dev/ttyUSB0 ]; then
    sudo chmod 666 /dev/ttyUSB0
fi

# Launch all nodes defined in the launch file
ros2 launch robotvehicle_package launch_vehicle_robot.py
