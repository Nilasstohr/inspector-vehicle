//
// Created by robot1 on 3/11/24.
//

#include "SensorData.h"

SensorData::SensorData(DriverInterface * driverInterface): driverInterface(driverInterface) {
    scanPolarForm = new std::vector<PointPolarForm>;
}

void SensorData::update(sensor_msgs::msg::LaserScan::SharedPtr currentScan) {
    scanPolarForm->clear();
    driverInterface->getWheelsTraveled(posLeft, posRight);
    int scanPointsNum = currentScan->scan_time / currentScan->time_increment;
    for(int i = 0; i < scanPointsNum; i++) {
        angle = (currentScan->angle_min + currentScan->angle_increment * i)+M_PI;
        distance = currentScan->ranges[i]*100;
        if(!std::isinf(distance)){
            scanPolarForm->push_back(PointPolarForm(angle,distance));
        }
    }
}

void SensorData::update(std::vector<PointPolarForm> * scan,double posLeft,double posRight) {
    scanPolarForm = scan;
    this->posLeft=posLeft;
    this->posRight=posRight;
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
