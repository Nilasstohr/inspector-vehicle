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
#include "../Sensor/SensorData.h"

#define POSE_STORAGE_SIZE 1300

class KalmanFilter: MatrixHelper {
public:
    KalmanFilter(SerialInterface * serialInterface);
    KalmanFilter();
    void update(sensor_msgs::msg::LaserScan::SharedPtr scan);
    void update(SensorData *sensorData);
    double getX();
    double getY();
    double getTheta();
    void printPoseStorage();
    bool reachedMaxPoseStorage();
    void build(sensor_msgs::msg::LaserScan::SharedPtr scan);
    int getLoggedNum();

    string * getPoseStringByIndex(int i);

private:
    void print(int index);
    void printPose(int index, const MatrixXd* m, char * text);
    void init();
    void update(std::vector<PointPolarForm> *scan,double posLeft,double right);
    PredictionDifferentialDrive * differentialDrive;
    PredictionDifferentialDrive * differentialDriveNoKalman;
    TestMap * testMap;
    MeasurementPrediction *measurementPrediction;
    Observations *observations;
    Matching * matching;
    Estimation *estimation;
    SensorData *sensorData;

    int xtCount;
    MatrixXd xEstBuffer[POSE_STORAGE_SIZE];
    MatrixXd xtBuffer[POSE_STORAGE_SIZE];
    string poseString;




};


#endif //ROBOTVEHICLE_PACKAGE_KALMANFILTER_H
