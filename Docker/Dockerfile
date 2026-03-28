# Base image: ROS2 Humble pre-installed (built from Docker/experimental/humble/)
FROM ros2-humble-image

# Working directory inside the container
ARG WD=/app

# Copy only the pre-built ROS2 packages (not the source code)
COPY ros2_ws/install $WD/install

# Copy the startup script that sources ROS2 and launches the nodes
COPY ros2_ws/Run.sh $WD/Run.sh

WORKDIR $WD

# When the container starts, run Run.sh
CMD ["bash", "Run.sh"]

