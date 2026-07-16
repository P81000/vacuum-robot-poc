# Vacuum Robot PoC (v0.0.1)

This is a PoC test project that started out as a fun experiment. It's not intended for production and is built entirely in my free time. There is a lot of work to be done to make this more complete and robust. 

Honestly, the main goal of this README is to document my own development steps so I don't get completely lost when I come back to this project after a few weeks without coding.

---

## Workarounds

Building an embedded Linux OS on my Mac required two massive workarounds. I had headaches trying to solve this bottlenecks:

1. **The Case-Sensitive Sparse Image (`yocto-disk.sparseimage`)**
    Mac's filesystem is case-insensitive by default. When I tried cloning Yocto directly to my disk, Git overwrote files with similar names and completely corrupted the source tree. My workaround was to put all the Yocto code inside a macOS Sparse Image formatted strictly as Case-Sensitive APFS.
2. **The `native-tmp` Directory**
   I noticed Docker Volume mounts on my Mac were way too slow for Bitbake and broke advanced Linux file permissions (sockets/symlinks). To fix this, I mapped Yocto's `TMPDIR` output to a native directory *inside* the container (`/home/yoctouser/native-tmp`). This entirely bypasses the Mac volume bridge for the heavy compilation output.

---

## Running it 

These are the exact sequential commands I used to bootstrap, compile, and run the entire architecture from scratch.

### 1. Virtual Disk
Create the case-sensitive virtual drive and mount it:
```bash
hdiutil create -type SPARSE -fs 'Case-sensitive APFS' -size 60g -volname YoctoBuild yocto-disk.sparseimage

open yocto-disk.sparseimage
```

### 2. Docker Builder
Build the Ubuntu environment and run it, mapping the virtual drive into the container:
```bash
docker build -t yocto-builder .
docker run -it --name yocto-builder -v /Volumes/YoctoBuild:/home/yoctouser/yocto-workspace yocto-builder
```

### 3. Yocto & ROS 2 Bootstrapping (Inside Docker)
Clone the necessary Kirkstone branches into the workspace:
```bash
cd yocto-workspace
git clone -b kirkstone git://git.yoctoproject.org/poky
git clone -b kirkstone git://git.openembedded.org/meta-openembedded
git clone -b kirkstone [https://github.com/ros/meta-ros.git](https://github.com/ros/meta-ros.git)
```
*Note: Custom layer -> `embedded-os` folder*

### 4. Configuration
Initialize the build environment and add the layers:
```bash
cd poky
source oe-init-build-env build-mac-m4

# Add standard and ROS layers
bitbake-layers add-layer ../meta-openembedded/meta-oe
bitbake-layers add-layer ../meta-openembedded/meta-python
bitbake-layers add-layer ../meta-openembedded/meta-networking
bitbake-layers add-layer ../meta-openembedded/meta-multimedia
bitbake-layers add-layer ../meta-ros/meta-ros-common
bitbake-layers add-layer ../meta-ros/meta-ros2
bitbake-layers add-layer ../meta-ros/meta-ros2-humble
bitbake-layers add-layer ../meta-vacuum
```

Append these specific settings to `conf/local.conf`:
```text
LICENSE_FLAGS_ACCEPTED += "commercial"
TMPDIR = "/home/yoctouser/native-tmp"
IMAGE_INSTALL:append = " iproute2 net-tools bash ros-core ros-base ros-humble-vacuum-navigator"
```

### 5. Compilation
Fire the engine:
```bash
bitbake core-image-minimal
```

### 6. Simulation 
**Terminal 1:**
Because Docker drops UDP packets, I use `socat` to catch QEMU's telemetry and bridge it to the Mac:
```bash
docker exec -it yocto-builder bash
socat UDP4-LISTEN:8080,fork UDP4:host.docker.internal:8080
```

**Terminal 2:**
It builds the Raylib visualizer
```bash
make build && make run
```

**Terminal 3:**
Boot QEMU and execute the ROS 2 node:
```bash
# Inside build-mac-m4
runqemu qemuarm64 nographic slirp

# Login as root, then:
source /opt/ros/humble/setup.bash
ros2 run vacuum_navigator vacuum_node

or if the command above dont work

export LD_LIBRARY_PATH=/opt/ros/humble/lib
. /opt/ros/humble/setup.sh
/opt/ros/humble/lib/vacuum_navigator/vacuum_node
```

---

## 🗺 TODOs

Right now, the robot just spins in a hardcoded circle. The goal is to build out a proper decoupled robotics architecture.

* [ ] **Implement Rigid Body Physics Mock:** Replace the hardcoded circle with a physics engine (Motor PID controllers, differential drive kinematics, collision boundaries).
* [ ] **Mock Hardware Sensor Streams:** Create mock interfaces for raw hardware data (e.g., IMU data over I2C/SPI, Odometry via UART).
* [ ] **Expand the ROS 2 Node Architecture:**
  * [ ] Add a Lidar Sensor Mock Node (Raycasting in the simulation).
  * [ ] Add a Navigation/Path Planning Node.
  * [ ] Add obstacles.
  * [ ] Add a State Machine Node (Cleaning, Searching for Dock, Error States).
* [ ] Add home station
