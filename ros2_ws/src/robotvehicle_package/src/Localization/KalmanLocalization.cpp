//
// Created by robot1 on 10/20/23.
//

#include <iostream>
#include "KalmanLocalization.h"



KalmanLocalization::KalmanLocalization() {
    init();
}

void KalmanLocalization::init(){
    differentialDrive =
            new PredictionDifferentialDrive();

    differentialDriveNoKalman =
            new PredictionDifferentialDrive();

    // for prediction step
    MatrixXd pEst(3, 3);
    double sigmaXY = 2; // cm
    double sigmaTheta = MathConversions::deg2rad(2); // degrees;
    pEst(0, 0)= pow(sigmaXY,2); pEst(0, 1)= 0; pEst(0, 2)= 0;
    pEst(1, 0)= 0; pEst(1, 1)= pow(sigmaXY,2); pEst(1, 2)= 0;
    pEst(2, 0)= 0; pEst(2, 1)= 0; pEst(2, 2)=  pow(sigmaTheta,2);
    //printMatrix(&pEst,"---Pest init---");
    MatrixXd xEst(3,1);
    xEst(0,0)=40;
    xEst(1,0)=40;
    xEst(2,0)=0;
    //printMatrix(pEst,"pEst");
    //printVector(xEst,"xEst");
    differentialDriveNoKalman->init(&xEst,&pEst);

    // for observations step
    double eps = 1;
    MatrixXd R(2, 2);
    R(0, 0)= pow(MathConversions::deg2rad(2),2); R(0, 1)= 0;
    R(1, 0)= 0; R(1, 1)= pow(2,2);
    observations = new Observations(eps,R);
    //printMatrix(&R,"----R init----");
    // measurement prediction
    measurementPrediction = new MeasurementPrediction(eps,R);

    // matching
    matching = new Matching(15);

    // Estimation
    estimation = new Estimation(xEst,pEst);

    pose = new Pose();
}

void KalmanLocalization::update(SensorData * sensorData) {
    //differentialDriveNoKalman->update(posLeft,posRight,
    //                      differentialDriveNoKalman->getXEst(),differentialDriveNoKalman->getPEst());
    //printMatrix(differentialDriveNoKalman->getXEst(),"---x_est (no kalman)--");
    //printMatrix(differentialDriveNoKalman->getPEst(),"---P_est (no kalman)--");
    differentialDrive->update(sensorData->getPosLeft(),sensorData->getPosRight(),estimation->getXt(),estimation->getPt());
    observations->update(sensorData->getScanPolarForm(),sensorData->getScanPolarForm()->size());
    measurementPrediction->update(differentialDrive);
    matching->update(differentialDrive,measurementPrediction,observations);
    estimation->update(matching,differentialDrive->getXEst(),differentialDrive->getPEst());
    measurementPrediction->reset();
    observations->reset();
    matching->reset();
}


Pose * KalmanLocalization::getPose() const{
    pose->update(estimation->getX(),estimation->getY(),estimation->getTheta());
}

void KalmanLocalization::build(SensorData * sensorData) {
    measurementPrediction->buildMap(sensorData->getScanPolarForm());
}

string *KalmanLocalization::getPoseLastString() {
    poseString.clear();
    poseString.append(to_string(estimation->getXt()->coeffRef(0,0))).append(" ");
    poseString.append(to_string(estimation->getXt()->coeffRef(1,0))).append(" ");
    poseString.append(to_string(estimation->getXt()->coeffRef(2,0)));
    return &poseString;
}

SensorData *KalmanLocalization::getSensorDate() {
    return sensorData;
}


