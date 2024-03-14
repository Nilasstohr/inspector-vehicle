//
// Created by robot1 on 3/11/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_SENSORDATA_H
#define ROBOTVEHICLE_PACKAGE_SENSORDATA_H


#include "PointPolarForm.h"
#include <cmath>
#include "../Utilities/SerialInterface.h"

class SensorData {
public:
    SensorData(SerialInterface *serialInterface);

    void update(sensor_msgs::msg::LaserScan::SharedPtr currentScan);

    std::vector<PointPolarForm> *getScanPolarForm() const;

    double getPosLeft() const;

    double getPosRight() const;

private:
    SerialInterface * serialInterface;
    std::vector<PointPolarForm> * scanPolarForm;
    double posLeft;
    double posRight;
    float angle;
    float distance;
    std::string *response;
};


#endif //ROBOTVEHICLE_PACKAGE_SENSORDATA_H
