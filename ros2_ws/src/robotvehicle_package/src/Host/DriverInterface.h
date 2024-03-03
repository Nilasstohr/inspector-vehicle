//
// Created by robot1 on 2/19/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_DRIVERINTERFACE_H
#define ROBOTVEHICLE_PACKAGE_DRIVERINTERFACE_H

#include "../Utilities/SerialInterface.h"

class DriverInterface {
public:
    DriverInterface(SerialInterface* serialInterface);
    void getWheelDisplacement(double &left, double &right);
    void reset();
private:
    SerialInterface* serialInterface;
    std::string *serialResponse;


};


#endif //ROBOTVEHICLE_PACKAGE_DRIVERINTERFACE_H
