//
// Created by robot1 on 11/1/24.
//

#ifndef CONTROLLERNODESTUB_H
#define CONTROLLERNODESTUB_H
#include "ControllerNode.h"
#include "TestUtilities/RecordHandler.h"
#include "ExceptionIncorrectEndPosition.h"

class ControllerNodeStub: public ControllerNode{
public:
    explicit ControllerNodeStub(SerialInterface * serialInterface);
private:
    void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan) override;
    void timer_callback() override;
    void verifyPosition(double currentPoseX, double currentPoseY);

    RecordHandler * recordHandler;
};



#endif //CONTROLLERNODESTUB_H
