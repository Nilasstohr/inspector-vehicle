#!/bin/bash
set -e
source /opt/ros/humble/setup.bash
colcon test --packages-select robotvehicle_package --ctest-args -R "TestNavigationUnitTest|LineDetectionIntegrationTest|ControllerNodeIntegrationTest"
colcon test-result --all --verbose
