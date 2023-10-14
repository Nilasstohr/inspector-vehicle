//
// Created by robot1 on 10/9/23.
//

#include <iostream>
#include "RStack.h"

RStack::RStack(int capacity): indexR(0),stackNum(0){
    Rt = MatrixXd(2*capacity,2*capacity);
    init();
}

void RStack::add(const MatrixXd *R) {
    //MatrixHelper::printMatrix(Rt,"Rt");
    for(int i=0; i<R->rows(); i++){
        for(int j=0; j<R->cols(); j++){
      //      cout << Rt->coeff(i , j) << " " << endl;
            Rt(i+indexR,j+indexR)=R->coeff(i , j);
        }
    }
    //printMatrix();
    indexR = indexR + 2;
    stackNum++;
}

void RStack::reset() {
    indexR=0;
    stackNum=0;
}

void RStack::printMatrix() {
    MatrixHelper::printMatrix(&Rt,"Rt");
}

const MatrixXd RStack::getRt() {
    return Rt.block(0,0,indexR,indexR);
}


void RStack::init() {
    for(int i=0; i<Rt.rows(); i++){
        for(int j=0; j<Rt.cols(); j++){
            //      cout << Rt->coeff(i , j) << " " << endl;
            Rt(i,j)=0;
        }
    }
}


