//
// Created by robot1 on 7/23/24.
//

#include "SensorRecord.h"

SensorRecord::SensorRecord(std::vector<PointPolarForm> *scan,double posLeft, double posRight):
scan(*scan),
posLeft(posLeft),
posRight(posRight){}

std::vector<PointPolarForm> *SensorRecord::getScan() {
    return &scan;
}

double SensorRecord::getPosLeft() {
    return posLeft;
}

double SensorRecord::getPosRight() {
    return posRight;
}
