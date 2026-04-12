//
// Created by robot1 on 11/1/24.
//

#include "DiffDriveBaseNode.h"
#include "tf2/LinearMath/Quaternion.h"
#include <fstream>
#include <string>
#include <Host/DriverInterface.h>


#define MIN_W_NORMAL 1
#define MAX_W_NORMAL 4
#define MIN_W_TURN -4

#define ROS_INFO RCUTILS_LOG_INFO

DiffDriveBaseNode::DiffDriveBaseNode(SerialInterface & serialInterface):
Node("tf_odom_node"),posLeft(0.0),posRight(0.0),linearX(0.0),angularZ(0.0)
{
    auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());

    odom = new Odom();

    driverInterface = new DriverInterface(serialInterface);

    tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
    odom_pub_ =this->create_publisher<nav_msgs::msg::Odometry>("/odom", 10);

    // Initialize TF2 listener with a 30-second cache to avoid "timestamp earlier than all data" drops
    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock(), tf2::durationFromSec(30.0));

    // f_listener_ is not called directly anywhere — it has no methods you invoke. Its sole purpose is to populate tf_buffer_ in the background.
    // It subscribes to the /tf and /tf_static topics and automatically stores every incoming transform into tf_buffer_.
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    if (serialInterface.hasResponse()) {
        ROS_INFO(serialInterface.getResponse()->c_str());
    }
    control_timer_ = this->create_wall_timer(std::chrono::milliseconds (15),
                                std::bind(&DiffDriveBaseNode::control_timer_callback, this));

    tf_odom_timer_ = this->create_wall_timer(std::chrono::milliseconds (10),
                                std::bind(&DiffDriveBaseNode::tf_odom_timer_callback, this));

    cmd_vel_ = this->create_subscription<geometry_msgs::msg::Twist>("/cmd_vel", default_qos,
    std::bind(&DiffDriveBaseNode::cmd_vel_subscriber_callback, this, std::placeholders::_1));

    serialInterface.sendRequest("r");
}

void DiffDriveBaseNode::control_timer_callback(){
    double tmpLeft = 0.0, tmpRight = 0.0;
    driverInterface->getWheelsTraveled(tmpLeft, tmpRight);
    {
        std::lock_guard lock_pos(pos_mutex);
        posLeft  = tmpLeft;
        posRight = tmpRight;
    }
    double linearX, angularZ;
    {
        std::lock_guard lock_vel(vel_mutex);
        linearX = this->linearX;
        angularZ =this->angularZ;
    }
    setVelocities( linearX,angularZ);
    // Print debug info here (slower 15ms timer) rather than in the 10ms TF timer
    // to avoid blocking TF publishing with the TF lookup inside printSlamAndNav2Data
    printSlamAndNav2Data();
}

void DiffDriveBaseNode::tf_odom_timer_callback(){
    double left, right;
    {
        std::lock_guard lock_pos(pos_mutex);
        left = posLeft;
        right = posRight;
    }
    odom->update(left, right);
    broadCastTF();
    publishOdom();
}

void DiffDriveBaseNode::cmd_vel_subscriber_callback(const geometry_msgs::msg::Twist::SharedPtr twist_msg) {
    std::lock_guard lock(vel_mutex);
    linearX  = twist_msg->linear.x;
    angularZ = twist_msg->angular.z;
}

void DiffDriveBaseNode::broadCastTF() const {
    geometry_msgs::msg::TransformStamped odom_tf;
    odom_tf.header.stamp = this->now();
    odom_tf.header.frame_id = "odom";
    odom_tf.child_frame_id = "base_link";

    odom_tf.transform.translation.x = odom->getX()/100;
    odom_tf.transform.translation.y = odom->getY()/100;
    odom_tf.transform.translation.z = 0.0;

    tf2::Quaternion q;
    q.setRPY(0, 0, odom->getTheta());
    odom_tf.transform.rotation.x = q.x();
    odom_tf.transform.rotation.y = q.y();
    odom_tf.transform.rotation.z = q.z();
    odom_tf.transform.rotation.w = q.w();

    tf_broadcaster_->sendTransform(odom_tf);
}

void DiffDriveBaseNode::publishOdom() const {
    nav_msgs::msg::Odometry odom_msg;
    odom_msg.header.stamp = this->now();
    odom_msg.header.frame_id = "odom";
    odom_msg.child_frame_id = "base_link";

    // Position
    odom_msg.pose.pose.position.x = odom->getX()/100;
    odom_msg.pose.pose.position.y = odom->getY()/100;
    odom_msg.pose.pose.position.z = 0.0;

    // Orientation
    tf2::Quaternion q;
    q.setRPY(0, 0, odom->getTheta());
    odom_msg.pose.pose.orientation.x = q.x();
    odom_msg.pose.pose.orientation.y = q.y();
    odom_msg.pose.pose.orientation.z = q.z();
    odom_msg.pose.pose.orientation.w = q.w();

    // Covariance from pEst (full 3x3 matrix mapped to 6x6)
    const MatrixXd* pEst = odom->getPEst();
    odom_msg.pose.covariance.fill(0.0);

    // Map 3x3 covariance to 6x6 (x, y, yaw are at indices 0, 1, 5)
    odom_msg.pose.covariance[0] = (*pEst)(0,0);    // x-x
    odom_msg.pose.covariance[1] = (*pEst)(0,1);    // x-y
    odom_msg.pose.covariance[5] = (*pEst)(0,2);    // x-yaw
    odom_msg.pose.covariance[6] = (*pEst)(1,0);    // y-x
    odom_msg.pose.covariance[7] = (*pEst)(1,1);    // y-y
    odom_msg.pose.covariance[11] = (*pEst)(1,2);   // y-yaw
    odom_msg.pose.covariance[30] = (*pEst)(2,0);   // yaw-x
    odom_msg.pose.covariance[31] = (*pEst)(2,1);   // yaw-y
    odom_msg.pose.covariance[35] = (*pEst)(2,2);   // yaw-yaw

    odom_pub_->publish(odom_msg);
}

void DiffDriveBaseNode::printSlamAndNav2Data() {
    // Get SLAM-corrected pose
    try {
        auto transform = tf_buffer_->lookupTransform("map", "base_link", tf2::TimePointZero);
        // Extract yaw from quaternion
        tf2::Quaternion q(
            transform.transform.rotation.x,
            transform.transform.rotation.y,
            transform.transform.rotation.z,
            transform.transform.rotation.w
        );
        tf2::Matrix3x3 m(q);
        double roll, pitch, yaw;
        m.getRPY(roll, pitch, yaw);

        RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 500,
            "SLAM Pose x: %.2f cm, y: %.2f cm, heading: %.4f rad",
            transform.transform.translation.x * 100.0,
            transform.transform.translation.y * 100.0,
            yaw);
    } catch (tf2::TransformException &ex) {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000,
            "Could not get SLAM pose: %s", ex.what());
    }
}

void DiffDriveBaseNode::setVelocities(double v, double w){
    static double l =0.145;
    static double r =0.045;
    if(w==0 && v==0){
        driverInterface->stop();
        return;
    }

    wl = double(v-w*l)/r;
    wr = double(v+w*l)/r;

     if(wl > MAX_W_NORMAL)
        wl = MAX_W_NORMAL;
     if(wr > MAX_W_NORMAL)
        wr = MAX_W_NORMAL;
    driverInterface->setAngularVelocity(wl,wr);
}
