//
// Created by robot1 on 11/1/24.
//

#ifndef CONTROLLERNODE_H
#define CONTROLLERNODE_H

#include "Localization/SensorRecorder.h"

#include "MissionController.h"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

class ControllerNode: public rclcpp::Node {
public:
    ControllerNode(SerialInterface *serialInterface);
protected:
    virtual void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan);
    virtual void timer_callback();
    rclcpp::TimerBase::SharedPtr timer_;
    MissionController * missionController;
private:
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laserScanSubscription_;
    sensor_msgs::msg::LaserScan::SharedPtr currentScan;
    bool scanReady;
    SensorRecorder * recorder;
};

#endif //CONTROLLERNODE_H
