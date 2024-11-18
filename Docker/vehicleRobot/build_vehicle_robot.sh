#!/bin/bash
docker stop $(docker ps -a -q)
docker rm $(docker ps -a -q)
docker builder prune --all
docker build  -t vehicle_robot_image .

