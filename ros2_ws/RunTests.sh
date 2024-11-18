#!/bin/bash
colcon test --packages-select robotvehicle_package 
colcon test-result --all --verbose
