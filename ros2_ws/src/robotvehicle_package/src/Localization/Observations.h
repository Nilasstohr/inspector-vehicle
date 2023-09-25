//
// Created by robot1 on 9/23/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_OBSERVATIONS_H
#define ROBOTVEHICLE_PACKAGE_OBSERVATIONS_H

#include "FeatureExstraction/Incremental.h"
#include "../Sensor/PointPolarForm.h"
#include <Eigen/Dense>
#include <vector>

using Eigen::MatrixXd;

class Observations {
public:
    Observations(double esp, const MatrixXd &r);
    void update(std::vector<PointPolarForm> *scan);
private:
    Incremental * incremental;
    double esp;
    MatrixXd R;
};


#endif //ROBOTVEHICLE_PACKAGE_OBSERVATIONS_H
