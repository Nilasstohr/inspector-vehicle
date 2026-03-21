//
// Created by robot1 on 1/24/26.
//

#ifndef DIFFDRIVEBASENODE_H
#define DIFFDRIVEBASENODE_H

#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <nav_msgs/msg/odometry.hpp>
#include <Utilities/SerialInterface.h>
#include <Odom.h>
#include <Sensor/SensorData.h>

class DriverInterface;

class   DiffDriveBaseNode final : public rclcpp::Node {
public:
    explicit DiffDriveBaseNode(SerialInterface *serialInterface);

private:
    void control_timer_callback();

    void tf_odom_timer_callback();

    void cmd_vel_subscriber_callback(const geometry_msgs::msg::Twist::SharedPtr twist_msg);

    void broadCastTF() const;

    void publishOdom() const;

    void setVelocities(double w, double v);

    rclcpp::TimerBase::SharedPtr control_timer_;
    rclcpp::TimerBase::SharedPtr tf_odom_timer_;

    void printSlamAndNav2Data();

    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_;

    Odom * odom;
    DriverInterface * driverInterface;

    double wl{};
    double wr{};

    std::mutex pos_mutex;
    double posLeft;
    double posRight;
    std::mutex vel_mutex;
    double linearX;
    double angularZ;

};


#endif //DIFFDRIVEBASENODE_H
