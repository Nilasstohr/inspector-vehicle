//
// Created by robot1 on 2/19/24.
//

#include "DriverInterface.h"

DriverInterface::DriverInterface(SerialInterface *serialInterface): serialInterface(serialInterface) {
    serialResponse = new std::string();
}

void DriverInterface::getWheelDisplacement(double &left,double &right) {
    serialInterface->sendRequest("p");
    serialResponse->append(serialInterface->getResponse()->c_str());
    //ROS_INFO(odomStr->c_str());
    left = std::stod(serialResponse->substr(0, serialResponse->find(" ")));
    right = std::stod(serialResponse->substr(serialResponse->find(" "), serialResponse->size()));
    serialResponse->clear();
}

void DriverInterface::reset() {
    serialInterface->sendRequest("r");
}
