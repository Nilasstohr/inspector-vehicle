//
// Created by robot1 on 3/11/24.
//

#include "SensorData.h"

SensorData::SensorData(SerialInterface * serialInterface): serialInterface(serialInterface) {
    scanPolarForm = new std::vector<PointPolarForm>;
    response = new std::string();
}

void SensorData::update(sensor_msgs::msg::LaserScan::SharedPtr currentScan) {
    scanPolarForm->clear();
    serialInterface->sendRequest("p");
    response->append(serialInterface->getResponse()->c_str());
    posLeft = std::stod(response->substr(0, response->find(" ")));
    posRight = std::stod(response->substr(response->find(" "), response->size()));
    response->clear();
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

