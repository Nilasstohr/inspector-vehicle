//
// Created by robot1 on 9/30/23.
//

#include <iostream>
#include "MeasurementPrediction.h"

MeasurementPrediction::MeasurementPrediction(double eps, const MatrixXd &R){
    observations = new Observations(eps,R);
    // initialize the H matrix vector with 2 x 3 space.
    H = MatrixXd(2,3);
}

void MeasurementPrediction::buildMap(std::vector<PointPolarForm> *scan,Pose * currentPose) {
    observations->update(scan,scan->size());
    //observations->printLineStack();
    // transform lines from robot to world reference
    for(int j=0; j<observations->size(); j++){
        addLineToMap(observations->getLines()->getLine(j),currentPose);
    }
    //observations->printLineStack();
    this->z_est  = new LineStack(3000);
    this->hStack = new HStack(3000);
}

void MeasurementPrediction::addLineToMap(Line * line,Pose * currentPose) {
    line->toGlobalReferenceFrame(currentPose);
    line->updateOriginLineNormal();
    lineWorldMap.addLine(line);
    //cout << line->getAlfa() << " " << line->getR() << endl;
}

void MeasurementPrediction::addLinesToMap(Lines * unmatchedLines, double x, double y, double theta) {
    Pose currentPose;
    currentPose.update(x,y,theta);
    for(int i=0; i<unmatchedLines->size(); i++) {
        addLineToMap(unmatchedLines->getLine(i),&currentPose);
    }
    //std::cout<<"addLinesToMap: "<<linesW->size()<<"\n";
}

void MeasurementPrediction::update(const PredictionDifferentialDrive *prediction) {
    double xESt;
    double yEst;
    double thetaEst;
    double alfaW;
    double rW;
    double alfaR;
    double rR;
    /*
    Lines lineWorldMap;
    lineWorldMap.addLine(60.979, -14503.5);
    lineWorldMap.addLine(-0.0249823, 180.085);
    lineWorldMap.addLine(0.313572,163.068);
    lineWorldMap.addLine(48.5323, 132.339);
    lineWorldMap.addLine(-1.00032, 140.125);
    lineWorldMap.addLine(-2.77891, 123.759);
    lineWorldMap.addLine(-0.813229, 101.981);
    lineWorldMap.addLine(0.62089, 94.9994);
    lineWorldMap.addLine(40.9292, 89.8273);
    lineWorldMap.addLine(-0.0412801, 1.71617);
    lineWorldMap.addLine(-0.687521, 181.108);
    lineWorldMap.addLine(-0.0312365, -1.13905);
    //lineWorldMap.printLineParameters2();
    */
    for(int j=0; j<lineWorldMap.size(); j++){
        alfaW = lineWorldMap.getLine(j)->getAlfa();
        rW = lineWorldMap.getLine(j)->getR();

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

Observations * MeasurementPrediction::getObservations() {
    return observations;
}

Lines * MeasurementPrediction::getMapLines() {
    return &lineWorldMap;
}

void MeasurementPrediction::print(const int j) const {
    cout << zEst(j)->coeff(0,0) << " " <<  zEst(j)->coeff(1,0);
}




