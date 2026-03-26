# What Changed on Your Docker System

## Summary

**Docker Desktop was removed** and replaced with **native Docker Engine**.  
Your user was added to the **docker group** so you can run Docker without `sudo`.

---

## Why?

Docker Desktop runs a **Linux virtual machine** under the hood.  
USB devices like `/dev/ttyUSB0` and `/dev/ttyACM0` live on the **host kernel**, not inside that VM.  
That's why `docker run --device=/dev/ttyUSB0` kept failing — the VM couldn't see the USB device.

**Native Docker Engine** runs directly on your Linux kernel, so it **can** access USB devices.

---

## What Was Done (step by step)

### 1. Docker Desktop was removed

- The Docker Desktop service was stopped.
- The `docker-desktop` package was uninstalled.
- Leftover files in `~/.docker/desktop` were deleted.

### 2. Native Docker Engine was installed

- Docker's official APT repository was added.
- These packages were installed: `docker-ce`, `docker-ce-cli`, `containerd.io`, `docker-buildx-plugin`.
- The Docker daemon now runs as a system service (`systemctl start docker`).

### 3. Your user was added to the `docker` group

Without this, only `root` can talk to the Docker daemon.  
The command was:

```bash
sudo usermod -aG docker robot1
```

This means you can run `docker` commands **without `sudo`**.  
You had to **log out and log back in** for this to take effect.

### 4. Docker context was switched

Docker Desktop creates its own context called `desktop-linux`.  
After uninstalling Desktop, the CLI was pointed back to the `default` context (the native daemon).

---

## How to Start Docker After a Reboot

```bash
sudo systemctl start docker
```

To make it start automatically on every boot:

```bash
sudo systemctl enable docker
```

---

## How to Build and Run Your Image

```bash
cd ~/devrepo/inspector-vehicle/Docker
./build_vehicle_robot.sh
./runImage.sh
```

---

## File Overview

| File | Purpose |
|---|---|
| `Docker/Dockerfile` | Builds the image: copies `ros2_ws/install` and `Run.sh` into `/app` |
| `Docker/build_vehicle_robot.sh` | Cleans old containers/cache, then builds the image |
| `Docker/runImage.sh` | Runs the image with USB devices (only those that exist) |
| `Docker/install_native_docker.sh` | The script that did the Desktop → native switch (already done, keep for reference) |

---

## If You Ever Want Docker Desktop Back

```bash
sudo apt install docker-desktop
```

But USB device passthrough will **not** work reliably with Docker Desktop on Linux.

