//
// Created by robot1 on 10/5/23.
//

#include <iostream>
#include "Matching.h"


Matching::Matching(int capacity) {
    matches = new Matches(capacity);
    vIJ = MatrixXd(2, 1);
    PIJ = MatrixXd(2, 2);
    g = 2;
}

void Matching::update(
        const PredictionDifferentialDrive *prediction,
        const MeasurementPrediction *measurementPrediction,
         Observations *observations)
{
    matches->reset();
    double d;
    bool matchFound = false;
    for(int i=0; i< observations->size(); i++){
        //cout << "------------------" << std::endl;
        //cout <<"Observ:"; observations->print(i); cout << std::endl;
        //cout << "------------------" << std::endl;
        for(int j=0; j<measurementPrediction->size(); j++){
            //cout <<"MeasPre:"; measurementPrediction->print(j);
            vIJ = *observations->z(i) - *measurementPrediction->zEst(j);
            //printMatrix(&vIJ,"vIJ");
            PIJ = *measurementPrediction->HEst(j) *
                  *prediction->getPEst() *
                  measurementPrediction->HEst(j)->transpose() +
                  * observations->R();
            //printMatrix(&PIJ,"PIJ");
            // mahalanobis distance
            d = (vIJ.transpose() * PIJ.inverse() * vIJ).coeff(0,0);
            //cout  << d << endl;
            if(d <= pow(g,2)){
                //cout <<" [match]";
                matches->add(&vIJ,measurementPrediction->HEst(j),observations->R());
                matchFound=true;
            }
            //cout << std::endl;
        }
        if(!matchFound) {
            unmatchedLines.addLine(observations->getLines()->getLine(i));
            if(const int count = observations->getLines()->getLine(i)->getPointCount(); count>CONFIG_MIN_POINTS_FOR_MAP_LINE){
               //std::cout<<"unmatched with " << count << " points added list"<< std::endl;
               unmatchedVerified.addLine(observations->getLines()->getLine(i));
            }
        }else {
            matchedLines.addLine(observations->getLines()->getLine(i));
        }
        matchFound = false;
    }
    //printMatrix(matches->getRt(),"R after set");
    //cout << "matches=" << matches->getMatchCount() << endl;
    //cout << "not matched=" << unmatched->size() << endl;
    matches->addXEst(prediction->getXEst());
    matches->addPEst(prediction->getPEst());
}

const Matches *Matching::getMatches() {
    return matches;
}

Lines * Matching::getUnmatchedVerified() {
    return &unmatchedVerified;
}


Lines * Matching::getUnMatchedLines() {
    return &unmatchedLines;
}
Lines * Matching::getMatchedLines() {
    return &matchedLines;
}

void Matching::reset() {
    matches->reset();
    matchedLines.reset();
    unmatchedLines.reset();
    unmatchedVerified.reset();
}


