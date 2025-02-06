//
// Created by robot1 on 10/5/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MATCHING_H
#define ROBOTVEHICLE_PACKAGE_MATCHING_H

#include "PredictionDifferentialDrive.h"
#include "MeasurementPrediction.h"
#include "Observations.h"
#include "Matches.h"
#include "LineStack.h"

class Matching: MatrixHelper {
public:
    Matching(int capacity);

    void update(const PredictionDifferentialDrive* prediction,
                const MeasurementPrediction* measurementPrediction,
                 Observations *observations);
    const Matches * getMatches();
    void reset();

    LineStack * getUnMatchedStack();
    Lines * getUnMatchedLines();
    Lines * getMatchedLines();

private:
    Matches * matches;
    LineStack *unmatched;
    MatrixXd vIJ;
    MatrixXd PIJ;
    double g;

    // for logging
    Lines matchedLines;
    Lines unmatchedLines;
};


#endif //ROBOTVEHICLE_PACKAGE_MATCHING_H
