//
// Created by robot1 on 9/30/23.
//

#include <iostream>
#include "MeasurementPrediction.h"

using namespace std;

MeasurementPrediction::MeasurementPrediction(const LineStack *linesW) {
    this->linesW = linesW;
    LineStack lines(linesW->size());
    linesR = &lines;
    // initilize the H matrix vector with 2 x 3 space.
    H = MatrixXd(2,3);
    hStack = HStack(0, linesW->size());

}


void MeasurementPrediction::update(const PredictionDifferentialDrive *prediction) {
    double x;
    double y;
    double alfaW;
    double rW;
    double alfaR;
    double rR;
    for(int j=0; j<linesW->size(); j++){
        alfaW = linesW->getAlfaByIndex(j);
        rW    = linesW->getRByIndex(j);
        alfaR = alfaW - prediction->getXEstLast()->z();
        x     = prediction->getXEstLast()->x();
        y     = prediction->getXEstLast()->y();
        rR    = rW - ( x*cos(alfaW)+y*sin(alfaW) );
        linesR->add(alfaR,rR);
        // row 1
        H(0,0)=0;
        H(0,1)=0;
        H(0,2)=-1;
        // row 2
        H(1,0)=-cos(alfaW);
        H(1,1)=-cos(alfaW);
        H(1,2)=0;



    }
}


