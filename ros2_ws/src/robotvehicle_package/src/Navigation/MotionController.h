//
// Created by robot1 on 2/18/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_MOTIONCONTROLLER_H
#define ROBOTVEHICLE_PACKAGE_MOTIONCONTROLLER_H

#include "../Host/DriverInterface.h"
#include "../Localization/KalmanFilter.h"

class MotionController {
public:
    MotionController(DriverInterface *driverInterface);
private:
    DriverInterface* driverInterface;
};

#endif //ROBOTVEHICLE_PACKAGE_MOTIONCONTROLLER_H
