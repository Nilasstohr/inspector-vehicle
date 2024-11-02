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

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

set (CMAKE_PREFIX_PATH "/home/robot1/Downloads/eigen-3.4.0")
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(controller_node
        src/controller_node.cpp
        src/MissionController.cpp
        src/MissionController.h
        src/ControllerNode.cpp
        src/ControllerNode.h
)
ament_target_dependencies(controller_node rclcpp sensor_msgs)

# add libraries
#--------------------------------
# declare variables and assign lib folder path to it (relative to root)
set(SrcRootPath               "src/")
set(HostLibPath               "src/Host/")
set(UtilitiesLibPath          "src/Utilities/")
set(SensorLibPath             "src/Sensor/")
set(LocalizationLibPath       "src/Localization/")
set(PathPlanningLibPath       "src/PathPlanning/")
set(PathNavigationLibPath     "src/Navigation/")
set(TestLibPath               "src/Test/")
# add libraries directory, these call actually executes CMakeLists.txt of the libs.
add_subdirectory(${UtilitiesLibPath})
add_subdirectory(${HostLibPath})
add_subdirectory(${SensorLibPath})
add_subdirectory(${LocalizationLibPath})
add_subdirectory(${PathPlanningLibPath})
add_subdirectory(${PathNavigationLibPath})
add_subdirectory(${TestLibPath})

# link the libraries to executable
target_link_libraries(controller_node
        HostLib
        SensorLib
        LocalizationLib
        PathPlanningLib
        NavigationLib
)

# add the libraries "include" files folders, so that include files can be found when referring to them.
target_include_directories(controller_node
        PUBLIC
        ${LocalizationLibPath} ${TestLibPath} ${PathNavigationLibPath}
)
# if library depend on a another library  include it to lib target as below
target_include_directories(HostLib PUBLIC  ${SrcRootPath})
target_include_directories(SensorLib PUBLIC  ${SrcRootPath})
target_include_directories(LocalizationLib  PUBLIC ${SrcRootPath})
target_include_directories(PathPlanningLib  PUBLIC   ${SrcRootPath})
target_include_directories(TestLib  PUBLIC  ${SrcRootPath})
target_include_directories(NavigationLib  PUBLIC  ${SrcRootPath})
#--------------------------------

add_executable(laserscansub_node src/laserscansub_node.cpp)
ament_target_dependencies(laserscansub_node rclcpp sensor_msgs)

install(TARGETS
  controller_node
  DESTINATION lib/${PROJECT_NAME})

install(FILES recordObst.txt
        DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  message(${CMAKE_CURRENT_BINARY_DIR} )

  # Add test utility library that other test can use.
  set(TestRootPath "test/")
  set(TestUtilitiesLibPath "${TestRootPath}/TestUtilities")
  add_subdirectory(${TestUtilitiesLibPath})
  target_include_directories(TestUtilitiesLib PUBLIC  ${SrcRootPath})

  # Testing Host Library
  ament_add_gtest(HostSystemTest ${TestRootPath}/Host/HostSystemTest.cpp)
  target_link_libraries(HostSystemTest HostLib UtilitiesLib)
  target_include_directories(HostSystemTest PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include>
          ${SrcRootPath} Configurations.h
  )

  # Testing Controller node
  # ! (notice a lot of repeated cmake code from controller_node executable above, a future task is to reuse here)
  ament_add_gtest(ControllerNodeIntegrationTest
          ${TestRootPath}/ControllerNode/ControllerNodeIntegrationTest.cpp
          ${TestRootPath}/ControllerNode/ControllerNodeStub.cpp
          ${TestRootPath}/ControllerNode/ControllerNodeStub.h
          ${SrcRootPath}/ControllerNode.cpp
          ${SrcRootPath}/ControllerNode.h
          ${SrcRootPath}/MissionController.cpp
          ${SrcRootPath}/MissionController.h
  )
  ament_target_dependencies(ControllerNodeIntegrationTest rclcpp sensor_msgs)
  target_link_libraries(ControllerNodeIntegrationTest TestUtilitiesLib
          HostLib
          SensorLib
          LocalizationLib
          PathPlanningLib
          NavigationLib)
  target_include_directories(ControllerNodeIntegrationTest PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include>
          ${SrcRootPath}  ${TestRootPath}  Configurations.h
  )

endif()

ament_package()
