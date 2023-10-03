//
// Created by robot1 on 10/1/23.
//

#include "MatrixStackBase.h"

MatrixStackBase::MatrixStackBase(int capacity) :
capacity(capacity),stackNum(0),index(0){}

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
    return 2*i;
}

int MatrixStackBase::mapIndexSecond(int i) const {
    return 2*i+1;
}