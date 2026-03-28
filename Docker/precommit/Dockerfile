# Base image: ROS2 Humble pre-installed (built from Docker/experimental/humble/)
FROM ros2-humble-image

# Copy the ROS2 workspace (includes source, Build.sh, RunPrecommitTests.sh)
COPY ros2_ws /ros2_ws

WORKDIR /ros2_ws

# Build first, then run the precommit tests
CMD ["bash", "-c", "bash Build.sh && bash RunPrecommitTests.sh"]
