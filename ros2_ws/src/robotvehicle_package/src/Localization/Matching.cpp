//
// Created by robot1 on 10/5/23.
//

#include <iostream>
#include "Matching.h"


Matching::Matching(int capacity)  {
    this->matches = new Matches(capacity);
    vIJ=MatrixXd (2,1);
    PIJ=MatrixXd (2,2);
    g=2;
}

void Matching::update(
        const PredictionDifferentialDrive *prediction,
        const MeasurementPrediction *measurementPrediction,
        const Observations *observations)
{
    matches->reset();
    double d;
    for(int i=0; i< observations->size(); i++){
        for(int j=0; j<measurementPrediction->size(); j++){
            vIJ = *observations->z(i) - *measurementPrediction->zEst(j);
            PIJ = *measurementPrediction->HEst(j) *
                  *prediction->getPEstLast()      *
                  measurementPrediction->HEst(j)->transpose() +
                  * observations->R();
            // mahalanobis distance
            d = (vIJ.transpose() * PIJ.inverse() * vIJ).coeff(0,0);
            //cout  << d << endl;
            if(d <= pow(g,2)){
                matches->add(&vIJ,measurementPrediction->HEst(j),observations->R());
            }

        }
    }
    matches->addXEst(prediction->getXEstLast());
    matches->addPEst(prediction->getPEstLast());
}

const Matches *Matching::getMatches() {
    return matches;
}
