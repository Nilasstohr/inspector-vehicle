/*
 * QuadratureEncoder.cpp
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#include "QuadratureEncoder.h"

QuadratureEncoder::QuadratureEncoder(QuadratureEncorderParameters *parameters) {
	this->parameters = parameters;
	this->_timeInterval = new QuadratureEncoderInterval();
	this->_countInterval = new QuadratureEncoderInterval();
	this->setupChannels();
}

void QuadratureEncoder::setupChannels() {
	// setup channel A
	pinMode(this->getParameters()->getPinChannelA(), INPUT);
	digitalWrite(this->getParameters()->getPinChannelA(), LOW);

	// setup channel B
	pinMode(this->getParameters()->getPinChannelB(), INPUT);
	digitalWrite(this->getParameters()->getPinChannelB(), LOW);

}

void QuadratureEncoder::channelAEventHandler() {
	uint8_t B = digitalRead(this->getParameters()->getPinChannelB());
	uint8_t A = digitalRead(this->getParameters()->getPinChannelA()) ;
	  if((A&&B)||(!A&&!B))
		counts--;
	  else
		counts++;
	this->refresh();
}

void QuadratureEncoder::channelBEventHandler() {
	uint8_t B = digitalRead(this->getParameters()->getPinChannelB()) ;
	uint8_t A = digitalRead(this->getParameters()->getPinChannelA()) ;
	if((B&&A)||(!B&&!A))
		counts++;
	else
		counts--;
	this->refresh();
}

uint16_t QuadratureEncoder::timeInterval() {
	return this->getTimeInterval()->get();
}

uint16_t QuadratureEncoder::countInterval() {
	return this->getPositionInterval()->get();
}

float QuadratureEncoder::velocity() {
	return (float)(this->getPositionInterval()->get()/this->getTimeInterval()->get());
}

void QuadratureEncoder::refresh() {
	uint64_t _micros = micros();
	// update time interval
	this->setPosition(this->getParameters()->countToCm(this->count()));
	this->getPositionInterval()->update((uint64_t)abs(this->count()));
	this->getTimeInterval()->update(_micros);
}

void QuadratureEncoder::reset() {
	this->getPositionInterval()->reset();
	this->getTimeInterval()->reset();
	this->counts = 0 ;
}

signed int QuadratureEncoder::count() {
	return this->counts;
}

QuadratureEncoder::~QuadratureEncoder() {
	// TODO Auto-generated destructor stub
}




