//
// Created by robot1 on 2/19/24.
//

#include "DriverInterface.h"

DriverInterface::DriverInterface(SerialInterface *serialInterface): serialInterface(serialInterface) {
    response = new std::string();
}

void DriverInterface::getWheelsTravled(double &left, double &right) {
    serialInterface->sendRequest("p");
    response->append(serialInterface->getResponse()->c_str());
    //ROS_INFO(odomStr->c_str());
    left = std::stod(response->substr(0, response->find(" ")));
    right = std::stod(response->substr(response->find(" "), response->size()));
    response->clear();
}

void DriverInterface::reset() {
    serialInterface->sendRequest("r");
}

void DriverInterface::setAngularVelocity(double wl, double wr) {
    request.clear();
    response->clear();
    request.append("v ");
    request.append(std::to_string(wl));
    request.append(" ");
    request.append(std::to_string(wr));
    request.append(";");
    serialInterface->sendRequest(&request);
}

void DriverInterface::stopAndResetDisplacement() {
    serialInterface->sendRequest("s");
    serialInterface->sendRequest("r");
}

void DriverInterface::stop() {
    serialInterface->sendRequest("s");
}
