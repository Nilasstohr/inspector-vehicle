//
// Created by robot1 on 11/1/24.
//

#ifndef CONTROLLERNODE_H
#define CONTROLLERNODE_H

#include "Localization/SensorRecorder.h"

#include "MissionController.h"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <nav_msgs/msg/odometry.hpp>

class ControllerNode: public rclcpp::Node {
public:
    ControllerNode(SerialInterface *serialInterface);
protected:
    virtual void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan);
    virtual void timer_callback();

    void broadCastTF() const;

    void publishOdom() const;

    void throwIfNotWithinRange(double current,double expected);

    rclcpp::TimerBase::SharedPtr timer_;
    MissionController * missionController;

private:
    void printSlamTecLocalization();
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laserScanSubscription_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    sensor_msgs::msg::LaserScan::SharedPtr currentScan;
    bool scanReady;
    SensorRecorder * recorder;
    NavigationPath * missionPath;
};

#endif //CONTROLLERNODE_H
