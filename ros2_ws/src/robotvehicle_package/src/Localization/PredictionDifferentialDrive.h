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
    void update(double sl, double sr, Vector3d xt, MatrixXd pt);
    const Vector3d *getXEstLast() const;
    const MatrixXd *getPEstLast() const;

private:
    double slLast;
    double srLast;
    Vector3d xEstLast;
    MatrixXd pEstLast;
    void setXEstLast(const Vector3d xEstLast);
    void setPEstLast(const MatrixXd pEstLast);
    void copyMatrix(const MatrixXd copyFrom,MatrixXd &copyTo);
    void copy3dVector(const Vector3d copyFrom,Vector3d copyTo);
};


#endif //ROBOTVEHICLE_PACKAGE_PREDICTIONDIFFERENTIALDRIVE_H
