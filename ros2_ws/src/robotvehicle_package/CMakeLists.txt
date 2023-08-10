cmake_minimum_required(VERSION 3.5)
project(robotvehicle_package)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set (CMAKE_PREFIX_PATH "/home/robot1/Downloads/eigen-3.4.0")
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_executable(laserscansub_node src/laserscansub_node.cpp)
ament_target_dependencies(laserscansub_node rclcpp sensor_msgs)

add_executable(
        controller_node
        src/controller_node.cpp
        src/SerialInterface.cpp
        src/SerialInterface.h
        src/UsbSerial.cpp
        src/UsbSerial.h
        src/AwaitTimer.cpp
        src/AwaitTimer.h src/OdomRangeLog.cpp src/OdomRangeLog.h src/LaserScanPoint.cpp src/LaserScanPoint.h)
ament_target_dependencies(controller_node rclcpp sensor_msgs)
target_link_libraries (controller_node Eigen3::Eigen)

install(TARGETS
  laserscansub_node
  controller_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
