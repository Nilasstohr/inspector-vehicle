//
// Created by robot1 on 2/19/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_DRIVERINTERFACE_H
#define ROBOTVEHICLE_PACKAGE_DRIVERINTERFACE_H

#include <Utilities/SerialInterface2.h>
#include "Utilities/SerialInterface.h"

class DriverInterface {
public:
    explicit DriverInterface(SerialInterface& serialInterface);
    void getWheelsTraveled(double &left, double &right);
    void reset();
    void setAngularVelocity(double wl, double wr);
    void stopAndResetDisplacement();
    void stop();
private:
    SerialInterface& serialInterface;
    SerialInterface2 serialInterface2;
    std::string response;
    std::string request;


};


#endif //ROBOTVEHICLE_PACKAGE_DRIVERINTERFACE_H
