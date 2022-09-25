/*
 * QuadratureEncorderParameters.cpp
 *
 *  Created on: Sep 21, 2022
 *      Author: robot1
 */

#include "QuadratureEncorderParameters.h"

QuadratureEncorderParameters::QuadratureEncorderParameters(
		uint8_t pinChannelA,
		uint8_t pinChannelB,
		uint8_t wheelDiameterCm,
		uint16_t countPrRevolution)
{
	this->pinChannelA =  pinChannelA;
	this->pinChannelB =  pinChannelB;

	// calculate count to cm position conversion number
	this->constantCountsToCm =  (double)(wheelDiameterCm*M_PI/countPrRevolution);
	// calculate radians pr count
	this->constantCountsToRadians = (double)(2*M_PI/countPrRevolution);
}

double QuadratureEncorderParameters::calculateCmFromCount(signed int count){
	return (double)(count*this->constantCountsToCm);
}

double QuadratureEncorderParameters::calculateAngularVelocity(uint32_t timeDiffMs) {
	return (double)(
	   this->constantCountsToRadians / this->msToSeconds(timeDiffMs)
	);
}

double QuadratureEncorderParameters::msToSeconds(uint32_t ms) {
	return (double)(ms/pow(10,6));
}

QuadratureEncorderParameters::~QuadratureEncorderParameters() {
	// TODO Auto-generated destructor stub
}


