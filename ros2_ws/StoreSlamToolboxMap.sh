#!/bin/bash
source /opt/ros/humble/setup.bash
ros2 service call /slam_toolbox/serialize_map slam_toolbox/srv/SerializePoseGraph "{filename: '/home/robot1/devrepo/inspector-vehicle/ros2_ws/my_map'}"

