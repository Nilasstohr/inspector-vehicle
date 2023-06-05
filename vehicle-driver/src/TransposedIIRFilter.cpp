/*
 * PiController.cpp
 *
 *  Created on: Dec 30, 2022
 *      Author: robot1
 */

#include <src/TransposedIIRFilter.h>

TransposedIIRFilter::TransposedIIRFilter(float b0, float b1, float a1):
b0(b0),
b1(b1),
a1(a1)
{
	reset();
}

double TransposedIIRFilter::update(double input) {
	output      = b0*input + feedForward - feedBack;
	/*
	Serial.print(" input= ");
	Serial.print(input);
	Serial.print(" ft= ");
	Serial.print(feedForward);
	Serial.print(" fb= ");
	Serial.print(feedBack);
	Serial.print(" o= ");
	Serial.print(output);
	*/
	feedForward = b1*input;
	feedBack    = a1*output;
	return output;
}

void TransposedIIRFilter::reset(){
	feedForward=0;
	feedBack=0;
	error=0;
	output=0;
}

double TransposedIIRFilter::get(){
	return output;
}

TransposedIIRFilter::~TransposedIIRFilter() {

}



