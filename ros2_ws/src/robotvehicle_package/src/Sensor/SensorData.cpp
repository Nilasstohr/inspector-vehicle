//
// Created by robot1 on 3/11/24.
//

#include "SensorData.h"

SensorData::SensorData(DriverInterface * driverInterface): driverInterface(driverInterface) {
    scanPolarForm = new std::vector<PointPolarForm>;
}

void SensorData::update(sensor_msgs::msg::LaserScan::SharedPtr currentScan) {
    scanPolarForm->clear();
    driverInterface->getWheelsTravled(posLeft, posRight);
    int scanPointsNum = currentScan->scan_time / currentScan->time_increment;
    for(int i = 0; i < scanPointsNum; i++) {
        angle = (currentScan->angle_min + currentScan->angle_increment * i)+M_PI;
        distance = currentScan->ranges[i]*100;
        if(!std::isinf(distance)){
            scanPolarForm->push_back(PointPolarForm(angle,distance));
        }
    }
}

std::vector<PointPolarForm> *SensorData::getScanPolarForm() const {
    return scanPolarForm;
}

double SensorData::getPosLeft() const {
    return posLeft;
}

double SensorData::getPosRight() const {
    return posRight;
}

