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
    void printMap();
    bool isMissionComplete() const;
    void setMissionPath(NavigationPath * navigation_path);

    KalmanLocalization *getLocalization() const;
    Odom * getOdom();

private:
    KalmanLocalization * localization;
    Odom * odom;
    Navigator * navigator;
    NavigationPath * missionPath;
    GridMap * gripMap;
    AStar * aStar;
    SensorData * sensorData;
    bool hasMapBeenBuild;
    bool obstacleAvoidanceInProgress;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr posePublisher_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr gridMapPublisher_;
    std_msgs::msg::String_<allocator<void>> posMessage = std_msgs::msg::String();
    bool missionComplete;

    void generateNewPathToDestination();
    void build();
    void updateMapAndPath(vector<PointPolarForm> * scan, Pose * pose);
    void updateMapWithObstacleSafeDistance();
};


#endif //ROBOTVEHICLE_PACKAGE_MISSIONCONTROLLER_H
