//
// Created by robot1 on 10/1/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MATRIXSTACKBASE_H
#define ROBOTVEHICLE_PACKAGE_MATRIXSTACKBASE_H

#include "MatrixHelper.h"

class MatrixStackBase: public MatrixHelper{
public:
    explicit MatrixStackBase(int capacity);
    int size() const;
    void reset();
    const MatrixXd *const getStack() const;
    int mapIndexFirst(int i) const;
    int mapIndexSecond(int i) const;
protected:
    int stackNum;
    int index;
    int capacity;
    MatrixXd stack;
};


#endif //ROBOTVEHICLE_PACKAGE_MATRIXSTACKBASE_H
