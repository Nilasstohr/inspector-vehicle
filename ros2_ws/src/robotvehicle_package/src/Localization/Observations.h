//
// Created by robot1 on 9/23/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_OBSERVATIONS_H
#define ROBOTVEHICLE_PACKAGE_OBSERVATIONS_H

#include "FeatureExstraction/Line.h"
#include "../Sensor/LaserScanPoint.h"
#include <Eigen/Dense>
#include <vector>

using Eigen::MatrixXd;

class Observations {
public:
    Observations(double esp, const MatrixXd &r);
    void update(std::vector<LaserScanPoint> *scan);
private:
    Line  * lines[100];
    double esp;
    MatrixXd R;
};


#endif //ROBOTVEHICLE_PACKAGE_OBSERVATIONS_H
