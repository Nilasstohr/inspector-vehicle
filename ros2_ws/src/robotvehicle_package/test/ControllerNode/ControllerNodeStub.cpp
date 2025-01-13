//
// Created by robot1 on 11/1/24.
//

#include "ControllerNodeStub.h"

#define EXPECTED_END_POSE_X 40
#define EXPECTED_END_POSE_Y 40
#define TOLERANCES_CM 3


ControllerNodeStub::ControllerNodeStub(SerialInterface *serialInterface):
ControllerNode(serialInterface) {
    recordHandler = new RecordHandler();
    missionController->resetRobotData();
}

void ControllerNodeStub::topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan) {
    (void)scan;
}

void ControllerNodeStub::timer_callback() {
    if(recordHandler->hasRecordsToProcess()){
        recordHandler->update(missionController->getSensorData());
        missionController->update();
        missionController->publishRobotData();
    }else{
        timer_->cancel();
        missionController->endMission();
        rclcpp::shutdown();
        verifyPosition(missionController->getCurrentPoseX(),missionController->getCurrentPoseY());
    }
}

void ControllerNodeStub::verifyPosition(double currentPoseX, double currentPoseY) {
    throwIfNotWithinRange(currentPoseX,EXPECTED_END_POSE_X);
    throwIfNotWithinRange(currentPoseY,EXPECTED_END_POSE_Y);
}

void ControllerNode::throwIfNotWithinRange(double current,double expected) {
    if(current>expected+TOLERANCES_CM || current<expected-TOLERANCES_CM) {
        throw ExceptionIncorrectEndPosition(current,expected,TOLERANCES_CM,__FILE__,__func__,__LINE__);
    }
}
