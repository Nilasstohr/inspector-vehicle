//
// Created by robot1 on 10/1/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MATRIXSTACKBASE_H
#define ROBOTVEHICLE_PACKAGE_MATRIXSTACKBASE_H

#include <Eigen/Dense>

using Eigen::MatrixXd;

class MatrixStackBase{
public:
    explicit MatrixStackBase(int capacity);
    int size();
protected:
    int stackNum;
    int index;
    int capacity;
};


#endif //ROBOTVEHICLE_PACKAGE_MATRIXSTACKBASE_H
