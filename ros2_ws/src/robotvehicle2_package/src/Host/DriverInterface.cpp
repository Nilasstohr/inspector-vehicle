//
// Created by robot1 on 2/19/24.
//

#include "DriverInterface.h"

DriverInterface::DriverInterface(SerialInterface& serialInterface): serialInterface(serialInterface) {}

void DriverInterface::getWheelsTraveled(double &left, double &right) {
    serialInterface.sendRequest("p");
    response.append(serialInterface.getResponse()->c_str());
    //ROS_INFO(odomStr->c_str());
    left = std::stod(response.substr(0, response.find(" ")));
    right = std::stod(response.substr(response.find(" "), response.size()));
    response.clear();
}

void DriverInterface::reset() {
    serialInterface.sendRequest("r");
}

void DriverInterface::setAngularVelocity(const double wl, const double wr) {
    request.clear();
    response.clear();
    request.append("v ");
    request.append(std::to_string(wl));
    request.append(" ");
    request.append(std::to_string(wr));
    serialInterface.sendRequest(&request);
}

void DriverInterface::stopAndResetDisplacement() const {
    serialInterface.sendRequest("s");
    serialInterface.sendRequest("r");
}

void DriverInterface::stop() const {
    serialInterface.sendRequest("s");
}

