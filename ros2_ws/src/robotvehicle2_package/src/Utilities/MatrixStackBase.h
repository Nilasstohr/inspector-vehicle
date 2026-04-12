//
// Created by robot1 on 10/1/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MATRIXSTACKBASE_H
#define ROBOTVEHICLE_PACKAGE_MATRIXSTACKBASE_H

#include "MatrixHelper.h"

class MatrixStackBase: public MatrixHelper{
public:
     MatrixStackBase(int capacity,int subSetRowsNum);
    int size() const;
    void reset();
    const MatrixXd * getStack() const;
    const MatrixXd getStackBlock(int rowSize, int colSize);
    int mapIndexFirst(int i) const;
    int mapIndexSecond(int i) const;
    void printMatrix(char *name);
    void printMatrix(char *name,int maxRows);
protected:
    int capacity;
    int stackNum;
    int index;
    int subSetRowsNum;
    MatrixXd stack;
};


#endif //ROBOTVEHICLE_PACKAGE_MATRIXSTACKBASE_H
