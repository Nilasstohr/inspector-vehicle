/*
 * averageCalc.cpp
 *
 *  Created on: Apr 7, 2023
 *      Author: robot1
 */

#include "AverageCalc.h"

AverageCalc::AverageCalc(int sampleSize)
{
	this->sampleSize= sampleSize;
	this->count = count;
	this->sum=0;
}

bool AverageCalc::add(uint32_t interval) {
	sum +=interval;
	count++;
	if(count>=sampleSize){
		return true;
	}
	return false;
}

uint32_t AverageCalc::average() {
	uint32_t intervalAverage = (uint32_t)(sum/sampleSize);
    sum=0;
    count=0;
    return intervalAverage;
}



