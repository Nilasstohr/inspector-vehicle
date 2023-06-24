/*
 * DualAccelerationControl.cpp
 *
 *  Created on: Jun 21, 2023
 *      Author: robot1
 */

#include "DualAccelerationControl.h"

DualAccelerationControl::DualAccelerationControl(double wheelRadius) {
	this->r = wheelRadius;
	this->alfa=0;
	this->w0=0;
	this->fi0=0;
}

void DualAccelerationControl::setAlfa(double dStart,double dEnd,double w0){
	this->w0=w0;
	fi0= dStart/r;
	alfa = -pow(w0,2)/(2*(dEnd/r-fi0));
}

double DualAccelerationControl::getVelocity(double d) {
	return sqrt( pow(w0,2)+2*alfa*(d/r-fi0) );
}

DualAccelerationControl::~DualAccelerationControl() {
	// TODO Auto-generated destructor stub
}

