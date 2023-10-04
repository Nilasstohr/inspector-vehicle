//
// Created by robot1 on 10/1/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MATRIXHELPER_H
#define ROBOTVEHICLE_PACKAGE_MATRIXHELPER_H

#include <Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::Vector3d;
using namespace std;

class MatrixHelper {
public:
    void printMatrix(const MatrixXd *matrix, char *name);
protected:
    void printVector(Vector3d *vector,char* name);

};


#endif //ROBOTVEHICLE_PACKAGE_MATRIXHELPER_H
