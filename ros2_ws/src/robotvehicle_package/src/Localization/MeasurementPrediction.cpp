//
// Created by robot1 on 9/30/23.
//

#include <iostream>
#include "MeasurementPrediction.h"

MeasurementPrediction::MeasurementPrediction(const LineStack *linesW) {
    this->linesW = linesW;
    this->z_est = new LineStack(linesW->size());
    this->hStack = new HStack(linesW->size());
    // initialize the H matrix vector with 2 x 3 space.
    H = MatrixXd(2,3);
}

MeasurementPrediction::MeasurementPrediction(double eps, const MatrixXd &R){
    observations = new Observations(eps,R);
    // initialize the H matrix vector with 2 x 3 space.
    H = MatrixXd(2,3);
}

void MeasurementPrediction::buildMap(std::vector<PointPolarForm> *scan) {
    observations->update(scan,scan->size());
    //observations->printLineStack();
    // transform lines from robot to world reference
    double alfaR;
    double rR;
    double alfaW;
    double rW;
    for(int j=0; j<observations->size(); j++){
        observations->getLinesStack()->getByIndex(j,alfaR,rR);
        transformToWorldReferenceFrame(alfaW,rW,40,40,0,alfaR,rR);
        observations->getLinesStack()->setByIndex(j,alfaW,rW);
    }

    //observations->printLineStack();
    linesW = observations->getLinesStack();
    this->z_est  = new LineStack(observations->size());
    this->hStack = new HStack(observations->size());
}

void MeasurementPrediction::update(const PredictionDifferentialDrive *prediction) {
    double xESt;
    double yEst;
    double thetaEst;
    double alfaW;
    double rW;
    double alfaR;
    double rR;
    for(int j=0; j<linesW->size(); j++){
        alfaW = linesW->getAlfaByIndex(j);
        rW = linesW->getRByIndex(j);

        xESt     = prediction->getX();
        yEst     = prediction->getY();
        thetaEst = prediction->getTheta();
        transformToRobotReferenceFrame(alfaW, rW, xESt, yEst, thetaEst, alfaR, rR);

        z_est->add(alfaR, rR);
        // row 1
        H(0,0)=0;
        H(0,1)=0;
        H(0,2)=-1;
        // row 2
        H(1,0)=-cos(alfaW);
        H(1,1)=-sin(alfaW);
        H(1,2)=0;
  //      printMatrix(&H,"H");
        hStack->add(&H);

    }
    //z_est->printLines("----z_est stacked (measurement prediction updateMapWithObstacleSafeDistance to {R} frame)----");
    //hStack->printMatrix("----H stacked----");
}

void MeasurementPrediction::transformToRobotReferenceFrame(
        double alfaW, double rW, double xEst, double yEst, double thetaEst, double &alfaR, double &rR){
    alfaR = alfaW - thetaEst;
    rR    = rW - (xEst * cos(alfaW) + yEst * sin(alfaW) );
    Line::correctPolarCoordinates(alfaR,rR);
}

void MeasurementPrediction::transformToWorldReferenceFrame(
        double &alfaW, double &rW, double xEst, double yEst, double thetaEst, double alfaR, double rR){
    alfaW = alfaR + thetaEst;
    rW    = rR + (xEst * cos(alfaW) + yEst * sin(alfaW) );
    Line::correctPolarCoordinates(alfaW,rW);
}
int MeasurementPrediction::size() const {
    return z_est->size();
}

const MatrixXd * MeasurementPrediction::zEst(int j) const {
    return z_est->getLineByIndex(j);
}

const MatrixXd *MeasurementPrediction::HEst(int j) const {
    return hStack->getHByIndex(j);
}

void MeasurementPrediction::reset() {
    z_est->reset();
    hStack->reset();
}




