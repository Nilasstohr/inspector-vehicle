//
// Created by robot1 on 9/30/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MEASUREMENTPREDICTION_H
#define ROBOTVEHICLE_PACKAGE_MEASUREMENTPREDICTION_H

#include "FeatureExstraction/Line.h"

class MeasurementPrediction {
public:

    MeasurementPrediction(const std::vector<Line> *lines);
private:
    std::vector<Line> *linesW;
    std::vector<Line> *linesR;
};


#endif //ROBOTVEHICLE_PACKAGE_MEASUREMENTPREDICTION_H
