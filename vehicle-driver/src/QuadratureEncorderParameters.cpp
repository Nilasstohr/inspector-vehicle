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
}

double QuadratureEncorderParameters::countToCm(uint64_t count) {
	return (double)(count*this->constantCountsToCm);
}

QuadratureEncorderParameters::~QuadratureEncorderParameters() {
	// TODO Auto-generated destructor stub
}


