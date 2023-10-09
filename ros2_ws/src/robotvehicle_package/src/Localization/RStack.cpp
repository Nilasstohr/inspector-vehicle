//
// Created by robot1 on 10/9/23.
//

#include <iostream>
#include "RStack.h"

RStack::RStack(int capacity): indexR(0){
    Rt = MatrixXd(capacity,capacity);
}

void RStack::add(MatrixXd *R) {
    //MatrixHelper::printMatrix(R,"R");
    for(int i=0; i<R->rows(); i++){
        for(int j=0; j<R->cols(); j++){
      //      cout << R->coeff(i , j) << " " << endl;
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


