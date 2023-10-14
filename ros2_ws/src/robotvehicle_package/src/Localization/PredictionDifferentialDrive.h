//
// Created by robot1 on 9/17/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_PREDICTIONDIFFERENTIALDRIVE_H
#define ROBOTVEHICLE_PACKAGE_PREDICTIONDIFFERENTIALDRIVE_H

#include <Eigen/Dense>
#include "../Utilities/MatrixHelper.h"

using Eigen::MatrixXd;
using Eigen::Matrix;
using Eigen::Vector3d;

class PredictionDifferentialDrive: MatrixHelper{
public:
    PredictionDifferentialDrive();
    void update(double sl, double sr,const MatrixXd * xt,const MatrixXd * Pt);
    const MatrixXd *getXEst() const;
    const MatrixXd *getPEst() const;

    double getTheta() const;

    double getX() const;

    double getY() const;

private:
    double slLast;
    double srLast;
    MatrixXd xEst;
    MatrixXd pEst;
    void setXEstLast(const MatrixXd xEstLast);
    void setPEstLast(const MatrixXd pEstLast);
    void copyMatrix(const MatrixXd copyFrom,MatrixXd &copyTo);
    void copy3dVector(const Vector3d copyFrom,Vector3d copyTo);
};


#endif //ROBOTVEHICLE_PACKAGE_PREDICTIONDIFFERENTIALDRIVE_H
