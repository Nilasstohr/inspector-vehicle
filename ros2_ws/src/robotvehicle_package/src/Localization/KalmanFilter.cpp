//
// Created by robot1 on 10/20/23.
//

#include <iostream>
#include "KalmanFilter.h"


KalmanFilter::KalmanFilter():xtCount(0) {
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
    MatrixXd xEst(3,1);
    xEst(0,0)=40;
    xEst(1,0)=40;
    xEst(2,0)=0;
    //printMatrix(pEst,"pEst");
    //printVector(xEst,"xEst");
    differentialDriveNoKalman->init(&xEst,&pEst);

    // measurement prediction
    testMap = new TestMap();
    measurementPrediction = new MeasurementPrediction(testMap->getMap());

    // for observations step
    double eps = 1;
    MatrixXd R(2, 2);
    R(0, 0)= pow(MathConversions::deg2rad(2),2); R(0, 1)= 0;
    R(1, 0)= 0; R(1, 1)= pow(2,2);
    observations = new Observations(eps,R);

    // matching
    matching = new Matching(15);

    // Estimation
    estimation = new Estimation(xEst,pEst);
}

void KalmanFilter::update(double odomLeft, double odomRight, std::vector<PointPolarForm> *scan) {
    differentialDriveNoKalman->update(odomLeft,odomRight,
                      differentialDriveNoKalman->getXEst(),differentialDriveNoKalman->getPEst());
    differentialDrive->update(odomLeft,odomRight,estimation->getXt(),estimation->getPt());
    measurementPrediction->update(differentialDrive);
    observations->update(scan,scan->size());
    matching->update(differentialDrive,measurementPrediction,observations);
    estimation->update(matching,differentialDrive->getXEst(),differentialDrive->getPEst());
    measurementPrediction->reset();
    observations->reset();
    matching->reset();
    // store
    xtBuffer[xtCount]  = *estimation->getXt();
    xEstBuffer[xtCount]= *differentialDriveNoKalman->getXEst();
    print(xtCount);
    xtCount++;
}



void KalmanFilter::printPose(int index, const MatrixXd* m, char * text) {
    //printMatrix(m,"m");
    cout << " " << text << ": "
         << m->coeffRef(0,0) << " "
         << m->coeffRef(1,0) << " "
         << m->coeffRef(2,0);
}

void KalmanFilter::print(int index) {
    cout << "(" << index+1 << ") ";
    printPose(xtCount, &xtBuffer[xtCount], "xt");
    printPose(xtCount, &xEstBuffer[xtCount], "xEst");
    cout << endl;
}

