//
// Created by robot1 on 10/9/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_RSTACK_H
#define ROBOTVEHICLE_PACKAGE_RSTACK_H


#include "../Utilities/MatrixHelper.h"

class RStack: public MatrixHelper{
public:
    RStack(int capacity);
    void add(MatrixXd * R);
    void reset();
    void printMatrix();
private:
    MatrixXd Rt;
    int indexR;
    int stackNum;
};


#endif //ROBOTVEHICLE_PACKAGE_RSTACK_H
