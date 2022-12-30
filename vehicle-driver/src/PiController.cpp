/*
 * PiController.cpp
 *
 *  Created on: Dec 30, 2022
 *      Author: robot1
 */

#include "PiController.h"

PiController::PiController(float b0, float b1, float a1):
b0(b0),
b1(b1),
a1(a1),
feedForward(0),
feedBack(0),
error(0),
output(0)
{}

double PiController::update(double input,double ref) {
	error       = ref - input;
	output      = b0*error + feedForward - feedBack;
	feedForward = b1*error;
	feedBack    = a1*output;
	return output;
}

PiController::~PiController() {
	// TODO Auto-generated destructor stub
}

