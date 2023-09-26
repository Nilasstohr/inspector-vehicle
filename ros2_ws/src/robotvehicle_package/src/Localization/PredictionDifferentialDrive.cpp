//
// Created by robot1 on 9/17/23.
//

#include <iostream>
#include "PredictionDifferentialDrive.h"
#include "rclcpp/rclcpp.hpp"
#define ROS_INFO RCUTILS_LOG_INFO

PredictionDifferentialDrive::PredictionDifferentialDrive():
srLast(0),slLast(0)
{

}

double PredictionDifferentialDrive::deg2rad(double deg) {
    return (deg * (PI / 180));
}

void PredictionDifferentialDrive::printMatrix(MatrixXd matrix,char * name) {
    ROS_INFO(name);
    for(int i=0; i<matrix.rows(); i++){
        for(int j=0; j<matrix.cols();j++){
            std::cout <<matrix(i,j);
            std::cout << " ";
        }
        std::cout <<std::endl;
    }
}

void PredictionDifferentialDrive::printVector(Eigen::Vector3d vector, char *name) {
    ROS_INFO(name);
    for(int i=0; i<3; i++){
        std::cout <<vector(i)<<std::endl;
    }
}

void PredictionDifferentialDrive::update(double sl, double sr, Vector3d xt, MatrixXd pt) {

    xEstLast=xt;
    pEstLast=pt;
    //setXEstLast(xt);
    //setXEstLast(pt);

    //printVector(xEstLast,"xEstLast");

    double dSl = sl -slLast;
    double dSr = sr -srLast;
    slLast = sl;
    srLast = sr;

    // distance between wheels in cm
    int b = 27;
    // angle of robot
    double phi = xEstLast.z();
    // Q covariance constants
    double kl  = 0.10;
    double kr  = 0.10;

    double dPhi = (dSr - dSl)/b;
    // distance traveled
    double dS   = (dSr + dSl)/2;

    // kinematic model for differential driver page 270
    Vector3d xEst = xEstLast + Vector3d (dS*cos(phi + dPhi/2),
                                         dS*sin(phi + dPhi/2),
                                         dPhi);

    //printVector(xEst,"xEst");

    MatrixXd Fx(3,3);
    Fx(0,0)= 1; Fx(0,1)= 0; Fx(0,2)= -dS*sin(phi + dPhi/2);
    Fx(1,0)= 0; Fx(1,1)= 1; Fx(1,2)=  dS*cos(phi + dPhi/2);
    Fx(2,0)= 0; Fx(2,1)= 0; Fx(2,2)=  1;

    MatrixXd Fu(3,2);
    // row 0
    Fu(0,0)=1/2*cos(phi + dPhi/2)-dS/(2*b)*sin(phi + dPhi/2);
    Fu(0,1)=1/2*cos(phi + dPhi/2)+dS/(2*b)*sin(phi + dPhi/2);
    // row 1
    Fu(1,0)=1/2*sin(phi + dPhi/2)+dS/(2*b)*cos(phi + dPhi/2);
    Fu(1,1)=1/2*sin(phi + dPhi/2)-dS/(2*b)*cos(phi + dPhi/2);
    // row 2
    Fu(2,0)=1/b;
    Fu(2,1)=-1/b;

    MatrixXd Q(2,2);
    Q(0,0)=kr*abs(dSr); Q(0,1)=0;
    Q(1,0)=0;              Q(1,1)=kl*abs(dSl);

    MatrixXd Pest(3,3);
    Pest = Fx * pEstLast * Fx.transpose() + Fu * Q * Fu.transpose();
    //printMatrix(Pest,"pEst");
    pEstLast = Pest;
    xEstLast = xEst;
}

const Vector3d PredictionDifferentialDrive::getXEstLast() const {
    return xEstLast;
}

const MatrixXd PredictionDifferentialDrive::getPEstLast() const {
    return pEstLast;
}

void PredictionDifferentialDrive::setXEstLast(const Vector3d xEstLast) {
    copy3dVector(xEstLast,this->xEstLast);
}

void PredictionDifferentialDrive::setPEstLast( MatrixXd pEstLast) {
    copyMatrix(pEstLast,this->pEstLast);
}

void PredictionDifferentialDrive::copyMatrix(const MatrixXd copyFrom, Eigen::MatrixXd &copyTo) {
    for(int i=0; i<copyFrom.rows(); i++){
        for(int j=0; j<copyFrom.cols();j++){
            copyTo(i,j) = copyFrom(i,j);
        }
        std::cout <<std::endl;
    }
}

void PredictionDifferentialDrive::copy3dVector(const Vector3d copyFrom, Eigen::Vector3d copyTo) {
    for(int i=0; i<3; i++){
        copyTo(i)=copyFrom(i);
    }
}