/*
 * QuadratureEncoder.cpp
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#include "QuadratureEncoder.h"


QuadratureEncoder::QuadratureEncoder(QuadratureEncorderParameters *parameters,TransposedIIRFilter *filter) {
	this->parameters = parameters;
	this->timeIntervalHandler = new QuadratureEncoderInterval();
	this->countIntervalHandler = new QuadratureEncoderInterval();
	this->timeMicros =0;
	this->filter = filter;
	this->filter->reset();
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
		counts++;
	  else
		counts--;
	this->refresh();
}

void QuadratureEncoder::channelBEventHandler() {
	uint8_t B = digitalRead(this->getParameters()->getPinChannelB()) ;
	uint8_t A = digitalRead(this->getParameters()->getPinChannelA()) ;
	if((B&&A)||(!B&&!A))
		counts--;
	else
		counts++;
	this->refresh();
}

uint32_t QuadratureEncoder::getTimeInterval() {
	return this->getTimeIntervalHandler()->get();
}

void QuadratureEncoder::updateVelocity() {
	this->filter->update(getTimeInterval());
}

void QuadratureEncoder::updatePosition() {
	position = getParameters()->calculateCmFromCount(count());
}

uint32_t QuadratureEncoder::getTimeIntervalFiltered() {
	return this->filter->get();
}


double QuadratureEncoder::getAngularVelocity() {
	return getParameters()->calculateAngularVelocity(getTimeIntervalFiltered());
}

double QuadratureEncoder::getPosition() {
	return position;
}



void QuadratureEncoder::refresh() {
	// update time interval
	//this->timeMicros =micros();
	getTimeIntervalHandler()->update(micros());

/*
	this->setAngularVelocity(
			this->getParameters()->calculateAngularVelocity(
					this->getTimeInterval()->get()));
*/
}

void QuadratureEncoder::reset() {
	this->getTimeIntervalHandler()->reset();
	this->counts = 0 ;
}

signed int QuadratureEncoder::count() {
	return this->counts;
}

QuadratureEncoder::~QuadratureEncoder() {
	// TODO Auto-generated destructor stub
}


