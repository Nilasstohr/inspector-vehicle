//
// Created by robot1 on 10/5/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MATCHING_H
#define ROBOTVEHICLE_PACKAGE_MATCHING_H

#include "PredictionDifferentialDrive.h"
#include "MeasurementPrediction.h"
#include "Observations.h"
#include "Matches.h"

class Matching: MatrixHelper {
public:
    Matching(int capacity);

    void update(const PredictionDifferentialDrive* prediction,
                const MeasurementPrediction* measurementPrediction,
                const Observations *observations);
private:
    Matches * matches;
    MatrixXd vIJ;
    MatrixXd PIJ;
    double g;
};


#endif //ROBOTVEHICLE_PACKAGE_MATCHING_H
