#!/bin/bash
# Serializes the current slam_toolbox pose graph to disk.
# The node writes two files:
#   my_map.posegraph   – pose graph used by slam_toolbox on next start
#   my_map.data        – associated scan/map data
#
# Run this BEFORE shutting down the slam_toolbox node.
source /opt/ros/humble/setup.bash

MAP_PATH=/home/robot1/devrepo/inspector-vehicle/ros2_ws/second_floor

echo "Saving map to: ${MAP_PATH}.*"
ros2 service call /slam_toolbox/serialize_map slam_toolbox/srv/SerializePoseGraph \
  "{filename: '${MAP_PATH}'}"

echo "Done. Files written:"
ls -lh "${MAP_PATH}".* 2>/dev/null || echo "  (check for second_floor.posegraph / second_floor.data)"

