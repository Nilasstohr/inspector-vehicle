//
// Created by robot1 on 9/30/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MEASUREMENTPREDICTION_H
#define ROBOTVEHICLE_PACKAGE_MEASUREMENTPREDICTION_H

#include "PredictionDifferentialDrive.h"
#include "LineStack.h"
#include "HStack.h"
#include "Observations.h"

class MeasurementPrediction: public MatrixHelper{
public:

    MeasurementPrediction(const LineStack *lines);

    MeasurementPrediction(double eps, const MatrixXd &r);
    void buildMap(std::vector<PointPolarForm> *scan,Pose * currentPose);

    void addLinesToMap(LineStack *unmatchedLines, double x, double y, double theta);

    void update(const PredictionDifferentialDrive *  prediction);
    void transformToRobotReferenceFrame(
            double alfaW, double rW, double xEst, double yEst, double thetaEst, double &alfaR, double &rR);
    void transformToWorldReferenceFrame(
            double &alfaW, double &rW, double xEst, double yEst, double thetaEst, double alfaR, double rR);
    const MatrixXd * zEst(int j) const;
    const MatrixXd * HEst(int j) const;
    int size() const;
    void reset();
    Observations * getObservations();
private:
    LineStack *linesW;
    Observations * observations;
    LineStack *z_est;
    HStack *hStack;
    MatrixXd H;
};


#endif //ROBOTVEHICLE_PACKAGE_MEASUREMENTPREDICTION_H
