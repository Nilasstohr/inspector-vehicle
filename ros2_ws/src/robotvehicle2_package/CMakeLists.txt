cmake_minimum_required(VERSION 3.5)
project(robotvehicle2_package)

# https://stackoverflow.com/questions/7900661/how-to-read-a-cmake-variable-in-c-source-code

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(std_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)


#set (CMAKE_PREFIX_PATH "/home/robot1/Downloads/eigen-3.4.0")
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(controller_node
        src/controller_node.cpp
        src/DiffDriveBaseNode.cpp
        src/DiffDriveBaseNode.h
)

add_executable(task_manager_node
        src/task_manager_node.cpp
        src/TaskManagerNode.cpp
        src/TaskManagerNode.h
        src/BehaviourTree/NavState.h
        src/BehaviourTree/IdleNode.h
        src/BehaviourTree/NavigateNode.h
        src/BehaviourTree/GoalReachedNode.h
)

target_include_directories(task_manager_node PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

ament_target_dependencies(controller_node rclcpp  nav_msgs tf2_ros)

ament_target_dependencies(task_manager_node rclcpp rclcpp_action nav_msgs nav2_msgs tf2 geometry_msgs behaviortree_cpp_v3)

#set(LIBINTERFACE_VERSION "\"helloWorld\"")

# add libraries
#--------------------------------
# declare variables and assign lib folder path to it (relative to root)
set(SrcRootPath               "src/")
set(HostLibPath               "src/Host/")
set(UtilitiesLibPath          "src/Utilities/")
set(EstimationLibPath       "src/Estimation/")

 # header template for configurations
  # set the variable which cmake should set in Configurations.h.in
  # generate the header which where the variables is set and used by source code.
  set(GeneratedFolder "${CMAKE_BINARY_DIR}/generated/")
  configure_file("${SrcRootPath}/Configurations.h.in" "${GeneratedFolder}/Configurations.h" )

# add libraries directory, these call actually executes CMakeLists.txt of the libs.
add_subdirectory(${UtilitiesLibPath})
add_subdirectory(${HostLibPath})
add_subdirectory(${EstimationLibPath})

# link the libraries to executable
target_link_libraries(controller_node
        HostLib
        EstimationLib
)

# add the libraries "include" files folders, so that include files can be found when referring to them.
target_include_directories(controller_node
        PUBLIC
        ${EstimationLibPath}
        ${GeneratedFolder}
)
# if library depend on a another library  include it to lib target as below
target_include_directories(HostLib PUBLIC  ${SrcRootPath} ${GeneratedFolder})
target_include_directories(EstimationLib  PUBLIC ${SrcRootPath} ${GeneratedFolder})
#--------------------------------

# resource files
set(ResourcesFolderName "Resources")
set(SrcResourcesFolder "${SrcRootPath}/${ResourcesFolderName}")

install(TARGETS
  controller_node
  task_manager_node
  DESTINATION lib/${PROJECT_NAME})
	
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}/)


#################### Testing ######################

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  message(${CMAKE_CURRENT_BINARY_DIR} )

  set(TestRootPath  "test/")
  set(TestResourcesFolder "${TestRootPath}/${ResourcesFolderName}")

  configure_file("${TestRootPath}/TestConfigurations.h.in" "${GeneratedFolder}/TestConfigurations.h")

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
