//
// Created by robot1 on 10/20/23.
//

#include <iostream>
#include "KalmanFilter.h"


KalmanFilter::KalmanFilter(SerialInterface *serialInterface){
    sensorData = new SensorData(serialInterface);
    init();
}

KalmanFilter::KalmanFilter() {
    init();
}

void KalmanFilter::init(){
    xtCount = 0;
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
    testMap = new TestMap();
    measurementPrediction = new MeasurementPrediction(eps,R);

    // matching
    matching = new Matching(15);

    // Estimation
    estimation = new Estimation(xEst,pEst);
}

void KalmanFilter::update(sensor_msgs::msg::LaserScan::SharedPtr scan) {
    sensorData->update(scan);
    update(sensorData->getScanPolarForm(),sensorData->getPosLeft(),sensorData->getPosRight());
}
void KalmanFilter::update(SensorData * sensorData) {
    update(sensorData->getScanPolarForm(),sensorData->getPosLeft(),sensorData->getPosRight());
}
void KalmanFilter::update(std::vector<PointPolarForm> *scan, double posLeft, double posRight) {
    differentialDriveNoKalman->update(posLeft,posRight,
                          differentialDriveNoKalman->getXEst(),differentialDriveNoKalman->getPEst());
    //printMatrix(differentialDriveNoKalman->getXEst(),"---x_est (no kalman)--");
    //printMatrix(differentialDriveNoKalman->getPEst(),"---P_est (no kalman)--");
    differentialDrive->update(posLeft,posRight,estimation->getXt(),estimation->getPt());
    observations->update(scan,scan->size());
    measurementPrediction->update(differentialDrive);
    matching->update(differentialDrive,measurementPrediction,observations);
    estimation->update(matching,differentialDrive->getXEst(),differentialDrive->getPEst());
    measurementPrediction->reset();
    observations->reset();
    matching->reset();
    // store
    xtBuffer[xtCount]  = *estimation->getXt();
    xEstBuffer[xtCount]= *differentialDriveNoKalman->getXEst();
    //print(xtCount);
    xtCount++;
}

void KalmanFilter::printPose(int index, const MatrixXd* m, char * text) {
    //printMatrix(m,"m");
    cout << m->coeffRef(0,0) << " "
         << m->coeffRef(1,0) << " "
         << m->coeffRef(2,0);
}

void KalmanFilter::print(int index) {
    //cout << "(" << index+1 << ") ";
    printPose(xtCount, &xtBuffer[index], "xt");
    //printPose(xtCount, &xEstBuffer[xtCount], "xEst");
    cout << endl;
}
void KalmanFilter::printPoseStorage(){
    cout << "count: "<< xtCount << endl;
    for(int i=0; i<xtCount; i++){
        print(i);
    }
}

double KalmanFilter::getY() {
    return estimation->getY();
}

double KalmanFilter::getX() {
    return estimation->getX();
}

double KalmanFilter::getTheta() {
    return estimation->getTheta();
}

void KalmanFilter::build(sensor_msgs::msg::LaserScan::SharedPtr scan) {
    sensorData->update(scan);
    measurementPrediction->buildMap(sensorData->getScanPolarForm());
}

bool KalmanFilter::reachedMaxPoseStorage() {
    return xtCount >= POSE_STORAGE_SIZE - 1;
}

int KalmanFilter::getLoggedNum() {
    return xtCount;
}

string *KalmanFilter::getPoseStringByIndex(int i) {
    poseString.clear();
    poseString.append(to_string(xtBuffer[i].coeffRef(0,0))).append(" ");
    poseString.append(to_string(xtBuffer[i].coeffRef(1,0))).append(" ");
    poseString.append(to_string(xtBuffer[i].coeffRef(2,0)));
    return &poseString;
}

