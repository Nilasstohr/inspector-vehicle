//
// Created by robot1 on 10/1/23.
//

#include "MatrixStackBase.h"

MatrixStackBase::MatrixStackBase(int capacity,int subSetRowsNum) :
capacity(capacity),stackNum(0),index(0),subSetRowsNum(subSetRowsNum){}

int MatrixStackBase::size() const {
    return stackNum;
}

void MatrixStackBase::reset() {
    stackNum=0;
    index=0;
}

const MatrixXd *const MatrixStackBase::getStack() const{
    return &stack;
}

int MatrixStackBase::mapIndexFirst(int i) const {
    return subSetRowsNum*i;
}

int MatrixStackBase::mapIndexSecond(int i) const {
    return subSetRowsNum*i+1;
}

void MatrixStackBase::printMatrix(char *name) {
    MatrixHelper::printMatrix(&stack,name);
}