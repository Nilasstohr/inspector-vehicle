//
// Created by robot1 on 10/20/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_KALMANLOCALIZATION_H
#define ROBOTVEHICLE_PACKAGE_KALMANLOCALIZATION_H

#include "PredictionDifferentialDrive.h"
#include "../Utilities/MathConversions.h"
#include "TestMap.h"
#include "MeasurementPrediction.h"
#include "Observations.h"
#include "Matching.h"
#include "Estimation.h"
#include "Pose.h"
#include "../Sensor/SensorData.h"
#include "../Host/DriverInterface.h"

class KalmanLocalization: MatrixHelper {
public:
    KalmanLocalization();
    KalmanLocalization(DriverInterface *driverInterface);

    void update(sensor_msgs::msg::LaserScan::SharedPtr scan);
    void update(SensorData *sensorData);
    void build(sensor_msgs::msg::LaserScan::SharedPtr scan);
    string *getPoseLastString();
    Pose * getPose() const;
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
    Pose * pose;
    string poseString;

};

#endif //ROBOTVEHICLE_PACKAGE_KALMANLOCALIZATION_H