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
	this->setupChannels();
}

void QuadratureEncoder::setupChannels() {
	// setup channel A
	pinMode(this->getPinChannelA(), INPUT);
	digitalWrite(this->getPinChannelA(), LOW);

	// setup channel B
	pinMode(this->getPinChannelB(), INPUT);
	digitalWrite(this->getPinChannelB(), LOW);

}

signed int QuadratureEncoder::getCount() {
	return this->counts;
}

void QuadratureEncoder::channelAEventHandler() {
	uint8_t B = digitalRead(this->pinChannelB);
	uint8_t A = digitalRead(this->pinChannelA) ;
	  if((A&&B)||(!A&&!B))
		counts--;
	  else
		counts++;
}

void QuadratureEncoder::channelBEventHandler() {
	uint8_t B = digitalRead(this->pinChannelB) ;
	uint8_t A = digitalRead(this->pinChannelA) ;
	if((B&&A)||(!B&&!A))
		counts++;
	else
		counts--;
}

QuadratureEncoder::~QuadratureEncoder() {
	// TODO Auto-generated destructor stub
}


