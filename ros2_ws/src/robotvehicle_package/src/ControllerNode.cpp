//
// Created by robot1 on 11/1/24.
//

#include "ControllerNode.h"
#include "tf2/LinearMath/Quaternion.h"
#include <fstream>
#include <string>

#define RECORD_DURATION_SECONDS 500

ControllerNode::ControllerNode(SerialInterface *serialInterface):
Node("reading_laser"),
scanReady(false)
{
    recorder = new SensorRecorder();
    missionPath = new NavigationPath();
    auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
    missionController = new MissionController(this,serialInterface);
    //missionPath->addPathPoint(40,200,0);
    missionPath->addPathPoint(170,65,0);
    missionPath->addPathPoint(40,40,0);
    //missionPath->addPathPoint(170,65,0);
    //missionPath->addPathPoint(40,40,0);
    missionController->setMissionPath(missionPath);
    recorder->startRecord(RECORD_DURATION_SECONDS);
    cout << "starting run" << endl;
    laserScanSubscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>("scan",default_qos,
            std::bind(&ControllerNode::topic_callback, this, placeholders::_1));

    tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
    odom_pub_ =this->create_publisher<nav_msgs::msg::Odometry>("/odom", 10);

    // Initialize TF2 listener
    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    timer_ = this->create_wall_timer(std::chrono::milliseconds (1),
                                std::bind(&ControllerNode::timer_callback, this));
}

void ControllerNode::topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan) {
    currentScan = scan;
    scanReady=true;
}

void ControllerNode::timer_callback(){
    if(scanReady) {
        scanReady = false;
        missionController->getSensorData()->update(currentScan);
        missionController->update();
        broadCastTF();
        publishOdom();
        printSlamTecLocalization();
        recorder->update(missionController->getSensorData());
        if(recorder->hasRecordTimeExceeded() || missionController->isMissionComplete()){
            timer_->cancel();
            laserScanSubscription_.reset();
            recorder->endRecord();
            missionController->endMission();
            rclcpp::shutdown();
        }
    }
}

void ControllerNode::broadCastTF() const {
    const Odom* odom = missionController->getOdom();
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

void ControllerNode::publishOdom() const {
    const Odom* odom = missionController->getOdom();

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

void ControllerNode::printSlamTecLocalization() {
    // Get SLAM-corrected pose
    try {
        auto transform = tf_buffer_->lookupTransform("map", "base_link", tf2::TimePointZero);
        cout << "SLAM Pose x: " << transform.transform.translation.x * 100.0
             << " cm, y: " << transform.transform.translation.y * 100.0 << " cm" << endl;
    } catch (tf2::TransformException &ex) {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000,
            "Could not get SLAM pose: %s", ex.what());
    }
}