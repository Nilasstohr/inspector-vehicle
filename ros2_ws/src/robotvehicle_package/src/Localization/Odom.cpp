//
// Created by robot1 on 9/17/23.
//

#include <iostream>
#include "Odom.h"
#include "rclcpp/rclcpp.hpp"
#define ROS_INFO RCUTILS_LOG_INFO

Odom::Odom()
{
    Pose startPose = Pose();
    startPose.update(CONFIG_START_POSE_X,CONFIG_START_POSE_Y,CONFIG_START_POSE_THETA);
    this->xEst = MatrixXd(3,1);
    this->pEst = MatrixXd(3,3);
    srLast=0;
    slLast=0;

    MatrixXd pEst(3, 3);
    double sigmaXY = 2; // cm
    double sigmaTheta = MathConversions::deg2rad(2); // degrees;
    pEst(0, 0)= pow(sigmaXY,2); pEst(0, 1)= 0; pEst(0, 2)= 0;
    pEst(1, 0)= 0; pEst(1, 1)= pow(sigmaXY,2); pEst(1, 2)= 0;
    pEst(2, 0)= 0; pEst(2, 1)= 0; pEst(2, 2)=  pow(sigmaTheta,2);
    //printMatrix(&pEst,"---Pest init---");
    MatrixXd xEst(3,1);
    xEst(0,0)=startPose.getX();
    xEst(1,0)=startPose.getY();
    xEst(2,0)=startPose.getTheta();

    copyMatrix(xEst,this->xEst);
    copyMatrix(pEst,this->pEst);
}


void Odom::update(double sl, double sr)
{
    //setXEstLast(xt);
    //setXEstLast(pt);
    //printMatrix(xt,"drive input --xt--");
    //printMatrix(Pt,"drive input --Pt--");

    //printVector(&xEst,"xEst");
    //cout << "---odometry (sr sl)----" << endl;
    //cout <<sl << " " << sr << endl;

    double dSl = sl -slLast;
    double dSr = sr -srLast;
    //cout << "dSl=" << dSl << " dSr=" << dSr << endl;
    slLast = sl;
    srLast = sr;

    // distance between wheels in cm
    int b = 27;
    // angle of robot
    double phi = xEst.coeff(2,0);
    // Q covariance constants
    double kl  = 0.10;
    double kr  = 0.10;

    double dPhi = (dSr - dSl)/b;
    // distance traveled
    double dS   = (dSr + dSl)/2;

    // kinematic model for differential driver page 270
    xEst = xEst + Vector3d (dS * cos(phi + dPhi / 2),dS*sin(phi + dPhi/2),dPhi);
    Line::limitAngle(xEst(2));
    //printMatrix(&xEst,"xEst");

    MatrixXd Fx(3,3);
    Fx(0,0)= 1; Fx(0,1)= 0; Fx(0,2)= -(double)dS*sin(phi + dPhi/2);
    Fx(1,0)= 0; Fx(1,1)= 1; Fx(1,2)=  (double)dS*cos(phi + dPhi/2);
    Fx(2,0)= 0; Fx(2,1)= 0; Fx(2,2)=  1;
    //printMatrix(&Fx,"---Fx---");

    MatrixXd Fu(3,2);

    // row 0
    Fu(0,0)=(double)1/2*cos(phi + dPhi/2)-(double)dS/(2*b)*sin(phi + dPhi/2);
    Fu(0,1)=(double)1/2*cos(phi + (double)dPhi/2)+dS/(2*b)*sin(phi + dPhi/2);

    // row 1
    Fu(1,0)=(double)1/2*sin(phi + dPhi/2)+(double)dS/(2*b)*cos(phi + dPhi/2) ;
    Fu(1,1)=(double)1/2*sin(phi + dPhi/2)-(double)dS/(2*b)*cos(phi + dPhi/2);

    // row 2
    Fu(2,0)= (double)1/b;
    Fu(2,1)= -(double)1/b;

    //printMatrix(&Fu,"---Fu---");

    MatrixXd Q(2,2);
    Q(0,0)=kr*abs(dSr); Q(0,1)=0;
    Q(1,0)=0;           Q(1,1)=kl*abs(dSl);
    //printMatrix(&Q,"---Q---");
    //printMatrix(Pt,"Pt");
    pEst = Fx * pEst * Fx.transpose() + Fu * Q * Fu.transpose();
    //printMatrix(&pEst,"pEst");
}

const MatrixXd *Odom::getXEst() const {
    return &xEst;
}

const MatrixXd * Odom::getPEst() const {
    return &pEst;
}

double Odom::getX() const {
    return getXEst()->coeff(0, 0);
}

double Odom::getY() const {
    return getXEst()->coeff(1, 0);
}

double Odom::getTheta() const {
    return getXEst()->coeff(2, 0);
}

void Odom::copyMatrix(const MatrixXd copyFrom, Eigen::MatrixXd &copyTo) {
    for(int i=0; i<copyFrom.rows(); i++){
        for(int j=0; j<copyFrom.cols();j++){
            copyTo(i,j) = copyFrom(i,j);
        }
    }
}




