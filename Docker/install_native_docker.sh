#!/bin/bash
# This script removes Docker Desktop and installs native Docker Engine.
# Native Docker runs directly on the host kernel, so USB devices like
# /dev/ttyUSB0 and /dev/ttyACM0 are accessible with --device flags.
#
# Run with: sudo bash install_native_docker.sh

set -e

echo "=== Step 1: Stop and remove Docker Desktop ==="
systemctl --user stop docker-desktop 2>/dev/null || true
sudo apt remove -y docker-desktop 2>/dev/null || true
rm -rf ~/.docker/desktop
sudo rm -f /usr/local/bin/com.docker.cli

echo ""
echo "=== Step 2: Install native Docker Engine ==="
sudo apt-get update
sudo apt-get install -y ca-certificates curl gnupg

sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg --yes
sudo chmod a+r /etc/apt/keyrings/docker.gpg

echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
  $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin

echo ""
echo "=== Step 3: Add current user to docker group ==="
sudo usermod -aG docker "$USER"

echo ""
echo "=== Step 4: Switch Docker CLI to native context ==="
docker context use default 2>/dev/null || true
docker context rm desktop-linux 2>/dev/null || true

# Remove stale Docker Desktop credential store reference
if [ -f "$HOME/.docker/config.json" ]; then
    sed -i '/"credsStore".*"desktop"/d' "$HOME/.docker/config.json"
    sed -i '/"currentContext".*"desktop-linux"/d' "$HOME/.docker/config.json"
fi

echo ""
echo "=== Done ==="
echo "Log out and log back in (or run 'newgrp docker') for group changes to take effect."
echo "Then rebuild and run your image with ./build_vehicle_robot.sh && ./runImage.sh"

