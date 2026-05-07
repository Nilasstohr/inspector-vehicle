//
// Created by robot1 on 2/19/24.
//

#include "DriverInterface.h"

#include <iostream>

DriverInterface::DriverInterface(SerialInterface& serialInterface): serialInterface(serialInterface),
                                                                    serialInterface2("/dev/ttyACM0", 115200) {}

void DriverInterface::getWheelsTraveled(double &left, double &right) {;
    serialInterface2.sendRequest("p");
    std::string response = serialInterface2.getResponse();
    //std::cout << "getWheelsTraveled response: '" << response << "'" << std::endl;
    left = std::stod(response.substr(0, response.find(" ")));
    right = std::stod(response.substr(response.find(" "), response.size()));
    response.clear();
}

void DriverInterface::reset() {
    serialInterface2.sendRequest("r");
}

void DriverInterface::setAngularVelocity(const double wl, const double wr) {
    std::string request;
    request.append("v ");
    request.append(std::to_string(wl));
    request.append(" ");
    request.append(std::to_string(wr));
    std::cout << "setAngularVelocity request: '" << request << "'" << std::endl;
    serialInterface2.sendRequest(request);
}

void DriverInterface::stopAndResetDisplacement() {
    serialInterface2.sendRequest("s");
    serialInterface2.sendRequest("r");
}

void DriverInterface::stop()  {
    serialInterface2.sendRequest("s");
}

