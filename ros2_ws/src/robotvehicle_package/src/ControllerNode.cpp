//
// Created by robot1 on 11/1/24.
//

#include "ControllerNode.h"
#include <fstream>
#include <string>

#define RECORD_DURATION_SECONDS 180

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
    //missionPath->addPathPoint(68,500,0);
    //missionPath->addPathPoint(40,40,0);

    missionController->setMissionPath(missionPath);
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
        if(recorder->hasRecordTimeExceeded() || missionController->isMissionComplete()){
            timer_->cancel();
            laserScanSubscription_.reset();
            recorder->endRecord();
            missionController->endMission();
            rclcpp::shutdown();
        }
    }
}
