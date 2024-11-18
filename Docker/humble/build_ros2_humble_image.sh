#!/bin/bash
docker stop $(docker ps -a -q)
docker rm $(docker ps -a -q)
docker builder prune --all
docker rmi -f $(docker images -aq)
docker build  -t ros2_humble_image .

