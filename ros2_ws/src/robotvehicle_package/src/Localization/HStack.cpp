//
// Created by robot1 on 10/1/23.
//

#include "HStack.h"

HStack::HStack(int capacity) : MatrixStackBase(capacity) {
    stack = MatrixXd(this->capacity * 2, 3);
}

void HStack::add(MatrixXd *H) {
    // row 1
    stack(index, 0)= H->coeff(0, 0);
    stack(index, 1)= H->coeff(0, 1);
    stack(index, 2)= H->coeff(0, 2);
    index++;
    stack(index, 0)= H->coeff(1, 0);
    stack(index, 1)= H->coeff(1, 1);
    stack(index, 2)= H->coeff(1, 2);
    index++;
    stackNum++;
}

MatrixXd *HStack::getHByIndex(int j) {
    // row 1
    this->H(0,0)=stack(mapIndexFirst(j), 0);
    this->H(0,1)=stack(mapIndexFirst(j), 1);
    this->H(0,2)=stack(mapIndexFirst(j), 2);
    this->H(1,0)=stack(mapIndexSecond(j), 0);
    this->H(1,1)=stack(mapIndexSecond(j), 1);
    this->H(1,1)=stack(mapIndexSecond(j), 2);
    return &H;
}
