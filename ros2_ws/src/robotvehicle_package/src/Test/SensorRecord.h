//
// Created by robot1 on 7/23/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_SENSORRECORD_H
#define ROBOTVEHICLE_PACKAGE_SENSORRECORD_H


#include "../Sensor/PointPolarForm.h"

class SensorRecord {
public:
    SensorRecord(std::vector<PointPolarForm> *scan,double posLeft, double posRight);

    std::vector<PointPolarForm> *getScan();

private:
    std::vector<PointPolarForm> scan;
    double posLeft;
public:
    double getPosLeft() ;

    double getPosRight();

private:
    double posRight;
};


#endif //ROBOTVEHICLE_PACKAGE_SENSORRECORD_H
