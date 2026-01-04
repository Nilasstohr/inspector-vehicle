cmake_minimum_required(VERSION 3.5)
project(robotvehicle_package)

# https://stackoverflow.com/questions/7900661/how-to-read-a-cmake-variable-in-c-source-code

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
find_package(nav_msgs REQUIRED)
find_package(tf2_ros REQUIRED)


#set (CMAKE_PREFIX_PATH "/home/robot1/Downloads/eigen-3.4.0")
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(controller_node
        src/controller_node.cpp
        src/MissionController.cpp
        src/MissionController.h
        src/ControllerNode.cpp
        src/ControllerNode.h
        test/ControllerNode/ExceptionIncorrectEndPosition.cpp
        test/ControllerNode/ExceptionIncorrectEndPosition.h
)
ament_target_dependencies(controller_node rclcpp sensor_msgs nav_msgs tf2_ros)

#set(LIBINTERFACE_VERSION "\"helloWorld\"")

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

# header template for configurations
  # set the variable which cmake should set in Configurations.h.in
  set(GripMapFileName "GridMap.txt")
  # generate the header which where the variables is set and used by source code.
  set(GeneratedFolder "${CMAKE_BINARY_DIR}/generated/")
  configure_file("${SrcRootPath}/Configurations.h.in" "${GeneratedFolder}/Configurations.h" )

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
        ${LocalizationLibPath}
        ${TestLibPath}
        ${PathNavigationLibPath}
        ${GeneratedFolder}
)
# if library depend on a another library  include it to lib target as below
target_include_directories(HostLib PUBLIC  ${SrcRootPath} ${GeneratedFolder})
target_include_directories(SensorLib PUBLIC  ${SrcRootPath} ${GeneratedFolder})
target_include_directories(LocalizationLib  PUBLIC ${SrcRootPath} ${GeneratedFolder})
target_include_directories(PathPlanningLib  PUBLIC   ${SrcRootPath} ${GeneratedFolder})
target_include_directories(TestLib  PUBLIC  ${SrcRootPath} ${GeneratedFolder})
target_include_directories(NavigationLib  PUBLIC  ${SrcRootPath} ${GeneratedFolder})
#--------------------------------

add_executable(laserscansub_node src/laserscansub_node.cpp)
ament_target_dependencies(laserscansub_node rclcpp sensor_msgs)

# resource files
set(ResourcesFolderName "Resources")
set(SrcResourcesFolder "${SrcRootPath}/${ResourcesFolderName}")
set(GridMapResourceFile "${SrcResourcesFolder}/${GripMapFileName}")

install(TARGETS
  controller_node
  DESTINATION lib/${PROJECT_NAME})
	
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}/)

#install(FILES ${GridMapResourceFile}
#        DESTINATION lib/${PROJECT_NAME})
# work around for transferring resource file to ros2 install folder
configure_file(${GridMapResourceFile} "../../" COPYONLY)


configure_file(${GridMapResourceFile} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)


#################### Testing ######################

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  message(${CMAKE_CURRENT_BINARY_DIR} )

  set(TestRootPath  "test/")
  set(TestResourcesFolder "${TestRootPath}/${ResourcesFolderName}")

  # set the variable which cmake should set in TestConfigurations.h.in
  set(RecordFileName "TestRecord.txt")
  set(MapLinesFileName "MapLines.txt")

  configure_file("${TestRootPath}/TestConfigurations.h.in" "${GeneratedFolder}/TestConfigurations.h")

  set(RecordFilePath "${TestResourcesFolder}/${RecordFileName}")
  configure_file(${RecordFilePath} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

  set(MapLinesPath "${TestResourcesFolder}/${MapLinesFileName}")
  configure_file(${MapLinesPath} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

  # Add test utility library that other test can use.
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
          ${TestRootPath}/ControllerNode/ExceptionIncorrectEndPosition.cpp
          ${TestRootPath}/ControllerNode/ExceptionIncorrectEndPosition.h
          ${SrcRootPath}/ControllerNode.cpp
          ${SrcRootPath}/ControllerNode.h
          ${SrcRootPath}/MissionController.cpp
          ${SrcRootPath}/MissionController.h
  )
  ament_target_dependencies(ControllerNodeIntegrationTest rclcpp sensor_msgs nav_msgs tf2_ros)
  target_link_libraries(ControllerNodeIntegrationTest TestUtilitiesLib
          HostLib
          SensorLib
          LocalizationLib
          PathPlanningLib
          NavigationLib)
  target_include_directories(ControllerNodeIntegrationTest PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include>
          ${SrcRootPath}  ${TestRootPath}  ${GeneratedFolder}
  )

  # Testing Localization
  ament_add_gtest(LineDetectionIntegrationTest
          ${TestRootPath}/Localization/LineDetectionIntegrationTest.cpp
          ${TestRootPath}/ControllerNode/ControllerNodeStub.cpp
          ${TestRootPath}/ControllerNode/ControllerNodeStub.h
          ${TestRootPath}/ControllerNode/ExceptionIncorrectEndPosition.cpp
          ${TestRootPath}/ControllerNode/ExceptionIncorrectEndPosition.h
          ${SrcRootPath}/ControllerNode.cpp
          ${SrcRootPath}/ControllerNode.h
          ${SrcRootPath}/MissionController.cpp
          ${SrcRootPath}/MissionController.h
  )
  ament_target_dependencies(LineDetectionIntegrationTest rclcpp sensor_msgs)
  target_link_libraries(LineDetectionIntegrationTest
          TestUtilitiesLib
          HostLib
          SensorLib
          LocalizationLib
          PathPlanningLib
          NavigationLib
  )
  target_include_directories(LineDetectionIntegrationTest PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include>
          ${SrcRootPath}  ${TestRootPath}  ${GeneratedFolder}
  )

  # Testing Navigation
  ament_add_gtest(TestNavigationUnitTest
          ${TestRootPath}/Navigator/TestNavigationUnitTest.cpp

  )
  target_link_libraries(TestNavigationUnitTest UtilitiesLib NavigationLib)
  target_include_directories(TestNavigationUnitTest PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include>
          ${SrcRootPath}  ${TestRootPath}
  )

  # Testing Utilities
  ament_add_gtest(StringUnitTest
          ${TestRootPath}/Utilities/StringUnitTest.cpp
  )
  target_link_libraries(StringUnitTest UtilitiesLib)
  target_include_directories(StringUnitTest PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include>
          ${SrcRootPath}  ${TestRootPath}
  )


endif()

ament_package()
