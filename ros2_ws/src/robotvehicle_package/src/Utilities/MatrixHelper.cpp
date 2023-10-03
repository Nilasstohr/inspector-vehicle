//
// Created by robot1 on 10/1/23.
//

#include <iostream>
#include "MatrixHelper.h"


void MatrixHelper::printMatrix(const MatrixXd *matrix, char * name) {
    cout << name << endl;
    for(int i=0; i<matrix->rows(); i++){
        for(int j=0; j<matrix->cols();j++){
            cout <<matrix->coeff(i,j);
            cout << " ";
        }
        std::cout <<std::endl;
    }
}

void MatrixHelper::printVector(Eigen::Vector3d *vector, char *name) {
    cout << name << endl;
    for(int i=0; i<3; i++){
        std::cout <<vector->coeff(i)<<std::endl;
    }
}