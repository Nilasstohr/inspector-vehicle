//
// Created by robot1 on 10/20/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_KALMANFILTER_H
#define ROBOTVEHICLE_PACKAGE_KALMANFILTER_H

#include "PredictionDifferentialDrive.h"
#include "../Utilities/MathConversions.h"
#include "TestMap.h"
#include "MeasurementPrediction.h"
#include "Observations.h"
#include "Matching.h"
#include "Estimation.h"

class KalmanFilter: MatrixHelper {
public:
    KalmanFilter();
    void update(double odomLeft, double odomRight, std::vector<PointPolarForm> *scan,bool doLogging);
    double getX();
    double getY();
    double getTheta();
private:
    void print(int index);
    void printPose(int index, const MatrixXd* m, char * text);
    PredictionDifferentialDrive * differentialDrive;
    PredictionDifferentialDrive * differentialDriveNoKalman;
    TestMap * testMap;
    MeasurementPrediction *measurementPrediction;
    Observations *observations;
    Matching * matching;
    Estimation *estimation;

    int xtCount;
    MatrixXd xEstBuffer[4000];
    MatrixXd xtBuffer[4000];
};


#endif //ROBOTVEHICLE_PACKAGE_KALMANFILTER_H
