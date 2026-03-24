#!/bin/bash

# Detect which USB devices are plugged in.
# /dev/ttyUSB0 = lidar, /dev/ttyACM0 = Teensy motor controller.
# We only pass devices that actually exist, otherwise Docker errors out.
DEVICES=""
for dev in /dev/ttyUSB0 /dev/ttyACM0; do
    if [ -e "$dev" ]; then
        DEVICES="$DEVICES --device=$dev"
    else
        echo "Warning: $dev not found, skipping"
    fi
done

# Run the container:
#   -it            : interactive terminal (so you can see output and Ctrl+C)
#   --privileged   : full access to host devices (needed for USB serial ports)
#   --network=host : share the host network (needed for ROS2 node discovery)
#   $DEVICES       : pass through the detected USB devices
docker run -it \
    --privileged \
    --network=host \
    $DEVICES \
    vehicle_robot_image
