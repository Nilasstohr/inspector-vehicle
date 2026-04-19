//
// Created by robot1 on 10/1/23.
//

#include <iostream>
#include <iomanip>
#include "MatrixHelper.h"


void MatrixHelper::printMatrix(const MatrixXd *matrix, char * name) {
    printMatrix(matrix,name, matrix->rows(),5);
}

void MatrixHelper::printMatrix(const MatrixXd *matrix, char * name,int precision) {
    printMatrix(matrix,name, matrix->rows(),precision);
}

void MatrixHelper::printMatrix(const MatrixXd *matrix, char * name,int maxRows,int precision) {
    cout << name << endl;
    for(int i=0; i<maxRows; i++){
        for(int j=0; j<matrix->cols();j++){
            cout <<matrix->coeff(i,j);
            cout << "    ";
        }
        std::cout << std::fixed;
        std::cout << std::setprecision(precision);
        std::cout <<std::endl;
    }
}



void MatrixHelper::printVector(Eigen::Vector3d *vector, char *name) {
    cout << name << endl;
    for(int i=0; i<3; i++){
        std::cout <<vector->coeff(i)<<std::endl;
    }
}