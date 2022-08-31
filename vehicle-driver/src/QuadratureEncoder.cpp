/*
 * QuadratureEncoder.cpp
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#include "QuadratureEncoder.h"

static CSyncEvent channelAEvent;
static CSyncEvent channelBEvent;

static void channel_a_isr(void){
	channelAEvent();
}

static void channel_b_isr(void){
	channelBEvent();
}

QuadratureEncoder::QuadratureEncoder(int pinChannelA, int pinChannelB) {
	this->pinChannelA = pinChannelA;
	this->pinChannelB = pinChannelB;
}

void QuadratureEncoder::setupChannels() {
	// setup channel A
	pinMode(this->getPinChannelA(), INPUT);
	digitalWrite(this->getPinChannelA(), LOW);
	channelAEvent.Bind(this,&QuadratureEncoder::channelAEventHandler);
	attachInterrupt(this->getPinChannelA(),channel_a_isr, CHANGE);
	// setup channel B
	pinMode(this->getPinChannelB(), INPUT);
	digitalWrite(this->getPinChannelB(), LOW);
	channelAEvent.Bind(this,&QuadratureEncoder::channelBEventHandler);
	attachInterrupt(this->getPinChannelB(),channel_b_isr, CHANGE);
}

signed int QuadratureEncoder::getCounts() {
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


