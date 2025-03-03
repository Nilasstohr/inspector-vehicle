//
// Created by robot1 on 9/23/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_OBSERVATIONS_H
#define ROBOTVEHICLE_PACKAGE_OBSERVATIONS_H

#include "Incremental.h"
#include "LineStack.h"
#include "Sensor/PointPolarForm.h"
#include <Eigen/Dense>
#include <vector>

using Eigen::MatrixXd;

class Observations{
public:
    Observations(double eps, const MatrixXd &r);
    void update(std::vector<PointPolarForm> *scan,int scanPointsNum);
    LineStack * getLinesStack();
    Lines *getLines();
    MatrixXd * z(int i) ;
    const MatrixXd * R() const;
    int size() const;
    void printLineStack();
    void reset();

    void print(int i);

private:
    Incremental * incremental;
    double esp;
    MatrixXd R_;
};


#endif //ROBOTVEHICLE_PACKAGE_OBSERVATIONS_H
