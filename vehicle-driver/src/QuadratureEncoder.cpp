/*
 * QuadratureEncoder.cpp
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#include "QuadratureEncoder.h"


QuadratureEncoder::QuadratureEncoder(int pinChannelA, int pinChannelB) {
	this->pinChannelA = pinChannelA;
	this->pinChannelB = pinChannelB;


}

void QuadratureEncoder::setupChannel() {
}

QuadratureEncoder::~QuadratureEncoder() {
	// TODO Auto-generated destructor stub
}

void QuadratureEncoder::setupChannel(int pinChannel) {
	pinMode(pinChannel, INPUT);
	digitalWrite(pinChannel, LOW);
	attachInterrupt(pinChannel, QuadratureEncoder::channelISR(), CHANGE);
}

void QuadratureEncoder::channelISR() {
}
