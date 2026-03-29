# Base image: ROS2 Humble pre-installed (built from Docker/experimental/humble/)
FROM ros2-humble-image

# Copy the ROS2 workspace (includes source, Build.sh, RunPrecommitTests.sh)
# Use /app to match the release container (Docker/Dockerfile uses ARG WD=/app).
# colcon build writes hardcoded absolute paths into install/setup.bash,
# so the build location must match where the release container will use install/.
COPY ros2_ws /app
COPY Docker /Docker

WORKDIR /app

# Build first, then run the precommit tests
CMD ["bash", "-c", "bash Build.sh && bash RunPrecommitTests.sh"]
