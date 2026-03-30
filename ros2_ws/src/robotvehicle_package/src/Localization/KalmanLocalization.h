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
#include <Utilities/MathConversions.h>
#include "Configurations.h"


class KalmanLocalization: MatrixHelper {
public:
    KalmanLocalization();
    void update(const std::vector<PointPolarForm> &lidarScanPolarPoints, double posLeft, double posRight);
    void build(const std::vector<PointPolarForm> & lidarScanPolarPoints);

    static MatrixXd makeObservationNoiseR();

    string *getPoseLastString();
    Pose * getPose() const;

    Observations *getObservations();
    Matching & getMatching();
    MeasurementPrediction & getMeasurementPrediction();

    const Pose * getStarPose();

    PredictionDifferentialDrive *getPrediction();
private:
    void init();
    PredictionDifferentialDrive differentialDrive;
    PredictionDifferentialDrive  differentialDriveNoKalman;
    MeasurementPrediction measurementPrediction;
    Observations observations;
    Matching  matching;
    Estimation *estimation;
    Pose * pose;
    string poseString;
    Pose startPose;

};

#endif //ROBOTVEHICLE_PACKAGE_KALMANLOCALIZATION_H
