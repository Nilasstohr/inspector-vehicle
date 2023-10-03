//
// Created by robot1 on 9/30/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MEASUREMENTPREDICTION_H
#define ROBOTVEHICLE_PACKAGE_MEASUREMENTPREDICTION_H

#include "FeatureExstraction/Line.h"
#include "PredictionDifferentialDrive.h"
#include "FeatureExstraction/LineStack.h"
#include "HStack.h"

class MeasurementPrediction: public MatrixHelper{
public:

    MeasurementPrediction(const LineStack *lines);
    void update(const PredictionDifferentialDrive *  prediction);

private:
    const LineStack *linesW;
    LineStack *linesR;
    HStack *hStack;
    MatrixXd H;
};


#endif //ROBOTVEHICLE_PACKAGE_MEASUREMENTPREDICTION_H
