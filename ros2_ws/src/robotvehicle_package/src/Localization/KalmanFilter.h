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

class KalmanFilter: MatrixHelper {
public:
    KalmanFilter(SerialInterface * serialInterface);
    KalmanFilter();
    void update(sensor_msgs::msg::LaserScan::SharedPtr scan);
    void update(SensorData *sensorData);
    double getX();
    double getY();
    void build(sensor_msgs::msg::LaserScan::SharedPtr scan);
    string *getPoseLastString();

private:
    void init();
    void update(std::vector<PointPolarForm> *scan,double posLeft,double right);
    PredictionDifferentialDrive * differentialDrive;
    PredictionDifferentialDrive * differentialDriveNoKalman;
    MeasurementPrediction *measurementPrediction;
    Observations *observations;
    Matching * matching;
    Estimation *estimation;
    SensorData *sensorData;
    string poseString;

};

#endif //ROBOTVEHICLE_PACKAGE_KALMANFILTER_H
