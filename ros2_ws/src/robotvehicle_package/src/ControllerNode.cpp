//
// Created by robot1 on 11/1/24.
//

#include "ControllerNode.h"

#define RECORD_DURATION_SECONDS 120

ControllerNode::ControllerNode(SerialInterface *serialInterface):
Node("reading_laser"),
scanReady(false)
{
    recorder = new SensorRecorder();
    auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
    missionController = new MissionController(this,serialInterface);
    recorder->startRecord(RECORD_DURATION_SECONDS);
    cout << "starting run" << endl;
    laserScanSubscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>("scan",default_qos,
            std::bind(&ControllerNode::topic_callback, this, placeholders::_1));
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
        recorder->update(missionController->getSensorData());
        if(recorder->hasRecordTimeExceeded() ||
           missionController->getNavigator()->isDestinationReached()){
            timer_->cancel();
            laserScanSubscription_.reset();
            recorder->endRecord();
            missionController->endMission();
            rclcpp::shutdown();
        }
    }
}
