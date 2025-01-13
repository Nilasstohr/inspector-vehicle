//
// Created by robot1 on 8/22/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_MISSIONCONTROLLER_H
#define ROBOTVEHICLE_PACKAGE_MISSIONCONTROLLER_H

#include "Utilities/SerialInterface.h"
#include "Localization/KalmanLocalization.h"
#include "PathPlanning/GridMap.h"
#include "Navigation/Navigator.h"
#include "PathPlanning/AStar.h"

#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/string.hpp>
#define ROS_INFO RCUTILS_LOG_INFO

class MissionController {
public:
    MissionController(rclcpp::Node *node,SerialInterface * serialInterface);
    SensorData *getSensorData();
    Navigator *getNavigator();
    void endMission();
    void update();
    double getCurrentPoseX();
    double getCurrentPoseY();
    void publishRobotData();
    void resetRobotData() const;
private:
    KalmanLocalization * localization;
    Navigator * navigator;
    GridMap * gripMap;
    AStar * aStar;
    SensorData * sensorData;
    bool hasMapBeenBuild;
    bool obstacleAvoidanceInProgress;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr posePublisher_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr gridMapPublisher_;
    std_msgs::msg::String_<allocator<void>> posMessage = std_msgs::msg::String();
    void generateNewPathToDestination(int x, int y);

    void build();

    void updateMapAndPath(vector<PointPolarForm> * scan, Pose * pose);
    void initiateNavigationPath();
    void updateMapWithObstacleSafeDistance();
};


#endif //ROBOTVEHICLE_PACKAGE_MISSIONCONTROLLER_H
