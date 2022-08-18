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
	//this->setupChannels();
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

void QuadratureEncoder::channelAEventHandler() {

}

void QuadratureEncoder::channelBEventHandler() {

}

QuadratureEncoder::~QuadratureEncoder() {
	// TODO Auto-generated destructor stub
}
