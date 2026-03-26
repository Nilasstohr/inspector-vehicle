#!/bin/bash
set -e

# Resolve paths so the script works regardless of where you call it from
SCRIPT_DIR=$(dirname "$(realpath "$0")")
PROJECT_ROOT="$SCRIPT_DIR/.."

# Clean up: stop and remove old containers so we don't get name conflicts.
# "2>/dev/null || true" means: if no containers exist, ignore the error.
docker stop $(docker ps -a -q) 2>/dev/null || true
docker rm $(docker ps -a -q) 2>/dev/null || true

# Clear Docker's build cache so we always get a fresh image.
# -f skips the "are you sure?" prompt.
docker builder prune --all -f

# Build the image.
# -f points to our Dockerfile.
# The build context is the project root so COPY can access ros2_ws/.
docker build -f "$SCRIPT_DIR/Dockerfile" -t vehicle_robot_image "$PROJECT_ROOT"

