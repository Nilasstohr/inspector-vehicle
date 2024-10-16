cmake_minimum_required(VERSION 3.8)
project(test_ros2_project)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(talker src/publisher_member_function.cpp)
ament_target_dependencies(talker rclcpp std_msgs)
# add libraries
#--------------------------------
  # declare variables and assign lib folder path to it (relative to root)
  set(LibPath1 "src/Lib1/")
  set(LibPath2 "src/Lib2/")
  # add libraries directory, these call actually executes CMakeLists.txt of the libs.
  add_subdirectory(${LibPath1})
  add_subdirectory(${LibPath2})
  # link the libraries to executable
  target_link_libraries(talker Lib1 Lib2)
  # add the libraries "include" files folders, so that include files can be found when referring to them.
  target_include_directories(talker PUBLIC ${LibPath1} ${LibPath2})
  # if library depend on a another library  include it to lib target as below
  target_include_directories(Lib1 PUBLIC ${LibPath2})
#--------------------------------

install(TARGETS
  talker
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(${PROJECT_NAME}_tutorial_test test/tutorial_test.cpp src/ClassToTest.cpp src/ClassToTest.h)

  target_include_directories(${PROJECT_NAME}_tutorial_test PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include>
          src
  )
endif()

ament_package()
