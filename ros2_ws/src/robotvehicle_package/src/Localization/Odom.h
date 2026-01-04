//
// Created by robot1 on 9/17/23.
//

#ifndef ODOM_H
#define ODOM_H

#include <Eigen/Dense>
#include "Utilities/MatrixHelper.h"
#include "Line.h"
#include <Utilities/MathConversions.h>
#include "Configurations.h"

using Eigen::MatrixXd;
using Eigen::Matrix;
using Eigen::Vector3d;

class Odom: MatrixHelper{
public:
    Odom();
    void update(double sl, double sr);
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
    void copyMatrix(const MatrixXd copyFrom,MatrixXd &copyTo);

};


#endif //ODOM_H
