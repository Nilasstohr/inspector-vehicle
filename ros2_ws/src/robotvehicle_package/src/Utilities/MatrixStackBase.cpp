//
// Created by robot1 on 10/1/23.
//

#include "MatrixStackBase.h"

MatrixStackBase::MatrixStackBase(int capacity,int subSetRowsNum) :
capacity(capacity),
stackNum(0),
index(0),
subSetRowsNum(subSetRowsNum){}

int MatrixStackBase::size() const {
    return stackNum;
}

void MatrixStackBase::reset() {
    stackNum=0;
    index=0;
}

const MatrixXd * MatrixStackBase::getStack() const{
    return &stack;
}

int MatrixStackBase::mapIndexFirst(int i) const {
    return subSetRowsNum*i;
}

int MatrixStackBase::mapIndexSecond(int i) const {
    return subSetRowsNum*i+1;
}

void MatrixStackBase::printMatrix(char *name) {
    printMatrix(name,stack.rows());
}

void MatrixStackBase::printMatrix(char *name,int maxRows) {
    MatrixHelper::printMatrix(&stack,name,maxRows);
}


const MatrixXd MatrixStackBase::getStackBlock(int rowSize,int colSize) {
    return stack.block(0,0,rowSize,colSize );
}
