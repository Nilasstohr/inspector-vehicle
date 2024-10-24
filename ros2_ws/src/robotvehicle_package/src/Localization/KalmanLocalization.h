//
// Created by robot1 on 10/20/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_KALMANLOCALIZATION_H
#define ROBOTVEHICLE_PACKAGE_KALMANLOCALIZATION_H

#include "PredictionDifferentialDrive.h"
#include "MeasurementPrediction.h"
#include "Observations.h"
#include "Matching.h"
#include "Estimation.h"
#include "Utilities/Pose.h"
#include "Sensor/SensorData.h"
#include <Utilities/MathConversions.h>


class KalmanLocalization: MatrixHelper {
public:
    KalmanLocalization();
    void update(SensorData *sensorData);
    void build(SensorData * sensorData);
    string *getPoseLastString();
    Pose * getPose() const;
    SensorData *getSensorDate();
private:
    void init();
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
