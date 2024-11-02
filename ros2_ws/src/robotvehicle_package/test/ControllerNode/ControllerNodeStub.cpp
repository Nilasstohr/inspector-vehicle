//
// Created by robot1 on 11/1/24.
//

#include "ControllerNodeStub.h"

ControllerNodeStub::ControllerNodeStub(SerialInterface *serialInterface):
ControllerNode(serialInterface) {
    recordHandler = new RecordHandler();
}

void ControllerNodeStub::topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan) {
    (void)scan;
}

void ControllerNodeStub::timer_callback() {
    if(recordHandler->hasRecordsToProcess()){
        recordHandler->update(missionController->getSensorData());
        missionController->update();
    }else{
        timer_->cancel();
        missionController->endMission();
        rclcpp::shutdown();
        return;
    }
}
