//
// Created by robot1 on 10/5/23.
//

#include "Matching.h"


Matching::Matching(int capacity)  {
    this->matches = new Matches(capacity);
    vIJ=MatrixXd (2,1);
    PIJ=MatrixXd (3,2);
}

void Matching::update(
        const PredictionDifferentialDrive *prediction,
        const MeasurementPrediction *measurementPrediction,
        const Observations *observations)
{
    for(int i=0; i< observations->size(); i++){
        for(int j=0; measurementPrediction->size(); j++){
            vIJ = *observations->z(i) - *measurementPrediction->zEst(j);
            PIJ = *measurementPrediction->HEst(j) *
                  *prediction->getPEstLast()      *
                  measurementPrediction->HEst(j)->transpose() +
                  * observations->R();
        }
    }
}
