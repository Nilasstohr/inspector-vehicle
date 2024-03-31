//
// Created by robot1 on 3/11/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_SENSORDATA_H
#define ROBOTVEHICLE_PACKAGE_SENSORDATA_H


#include "PointPolarForm.h"
#include <cmath>
#include "../Host/DriverInterface.h"

class SensorData {
public:
    SensorData(DriverInterface *driverInterface);

    void update(sensor_msgs::msg::LaserScan::SharedPtr currentScan);

    std::vector<PointPolarForm> *getScanPolarForm() const;

    double getPosLeft() const;

    double getPosRight() const;

private:
    DriverInterface * driverInterface;
    std::vector<PointPolarForm> * scanPolarForm;
    double posLeft;
    double posRight;
    float angle;
    float distance;
};


#endif //ROBOTVEHICLE_PACKAGE_SENSORDATA_H
