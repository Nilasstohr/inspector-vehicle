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
find_package(std_msgs REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(tutorial_interfaces REQUIRED)
find_package(custom_interfaces REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_executable(laserscansub_node src/laserscansub_node.cpp)
ament_target_dependencies(laserscansub_node rclcpp sensor_msgs)

add_executable(
        controller_node
        src/controller_node.cpp
        src/Utilities/SerialInterface.cpp
        src/Utilities/SerialInterface.h
        src/Utilities/UsbSerial.cpp
        src/Utilities/UsbSerial.h
        src/Utilities/AwaitTimer.cpp
        src/Utilities/AwaitTimer.h
        src/Sensor/PointPolarForm.cpp
        src/Sensor/PointPolarForm.h
        src/Localization/PredictionDifferentialDrive.cpp
        src/Localization/PredictionDifferentialDrive.h
        src/Localization/Observations.cpp
        src/Localization/Observations.h
        src/Localization/FeatureExstraction/Line.cpp
        src/Localization/FeatureExstraction/Line.h
        src/Localization/FeatureExstraction/Incremental.cpp
        src/Localization/FeatureExstraction/Incremental.h
        src/Sensor/PointRectForm.cpp
        src/Sensor/PointRectForm.h
        src/Utilities/MathConversions.cpp
        src/Utilities/MathConversions.h 
        src/Localization/TestMap.cpp 
        src/Localization/TestMap.h 
        src/Localization/MeasurementPrediction.cpp 
        src/Localization/MeasurementPrediction.h 
        src/Localization/FeatureExstraction/LineStack.cpp 
        src/Localization/FeatureExstraction/LineStack.h 
        src/Localization/HStack.cpp 
        src/Localization/HStack.h 
        src/Utilities/MatrixStackBase.cpp 
        src/Utilities/MatrixStackBase.h 
        src/Utilities/MatrixHelper.cpp 
        src/Utilities/MatrixHelper.h 
        src/Localization/Matching.cpp 
        src/Localization/Matching.h 
        src/Localization/Matches.cpp 
        src/Localization/Matches.h 
        src/Localization/Estimation.cpp 
        src/Localization/Estimation.h 
        src/Localization/RStack.cpp 
        src/Localization/RStack.h
        src/Localization/KalmanLocalization.cpp
        src/Localization/KalmanLocalization.h
        src/Test/OffLineTesting.cpp
        src/Test/OffLineTesting.h
        src/Navigation/NavigationPath.cpp
        src/Navigation/NavigationPath.h
        src/Navigation/NavigationPoint.cpp
        src/Navigation/NavigationPoint.h
        src/Host/DriverInterface.cpp
        src/Host/DriverInterface.h
        src/Sensor/SensorRecorder.cpp
        src/Sensor/SensorRecorder.h
        src/Sensor/SensorData.cpp
        src/Sensor/SensorData.h
        src/Utilities/Pose.cpp
        src/Utilities/Pose.h
        src/Navigation/Navigator.cpp
        src/Navigation/Navigator.h
        src/PathPlanning/PathPoint.cpp
        src/PathPlanning/PathPoint.h
        src/PathPlanning/TestSearchAlgoritms.cpp
        src/PathPlanning/TestSearchAlgoritms.h
        src/PathPlanning/AStar.cpp
        src/PathPlanning/AStar.h
        src/Configurations.h src/Utilities/Transformations.cpp src/Utilities/Transformations.h src/PathPlanning/GridMap.cpp src/PathPlanning/GridMap.h src/PathPlanning/LinePoints.cpp src/PathPlanning/LinePoints.h src/Test/SensorRecord.cpp src/Test/SensorRecord.h)
        
ament_target_dependencies(
        controller_node
        rclcpp
        sensor_msgs
        tutorial_interfaces
        custom_interfaces
)

add_executable(
        joystick_node
        src/Utilities/SerialInterface.cpp
        src/Utilities/SerialInterface.h
        src/Utilities/UsbSerial.cpp
        src/Utilities/UsbSerial.h
        src/Utilities/AwaitTimer.cpp
        src/Utilities/AwaitTimer.h
        src/publisher_joystick_node.cpp)

ament_target_dependencies(
        joystick_node
        rclcpp
        std_msgs)

# add Feature extraction library
#add_subdirectory(FeatureExtraction)
#target_include_directories(controller_node PUBLIC FeatureExtraction)
#target_link_directories(controller_node PRIVATE FeatureExtraction)

# add Localization library
#add_subdirectory(Localization)
#target_include_directories(controller_node PUBLIC Localization)
#target_link_directories(controller_node PRIVATE Localization)

target_link_libraries (controller_node Eigen3::Eigen)

install(TARGETS
  laserscansub_node
  controller_node
  joystick_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
