/*
 * QuadratureEncoders.cpp
 *
 *  Created on: Sep 4, 2022
 *      Author: robot1
 */

#include "QuadratureEncoders.h"

static CSyncEvent channelA1Event;
static CSyncEvent channelB1Event;
static CSyncEvent channelA2Event;
static CSyncEvent channelB2Event;


static void channel_a_isr_1(void){
	channelA1Event();
}

static void channel_b_isr_1(void){
	channelB1Event();
}

static void channel_a_isr_2(void){
	channelA2Event();
}

static void channel_b_isr_2(void){
	channelB2Event();
}

QuadratureEncoders::QuadratureEncoders(int channel1A,int channel1B,int channel2A,int channel2B)
{
	this->quadratureEncoderLeft = new QuadratureEncoder(channel1A,channel1B);
	this->quadratureEncoderRight = new QuadratureEncoder(channel2A,channel2B);
}

void QuadratureEncoders::setupEncoders() {

	// encoder 1

	channelA1Event.Bind(this->quadratureEncoderLeft,&QuadratureEncoder::channelAEventHandler);
	attachInterrupt(this->quadratureEncoderLeft->getPinChannelA(),channel_a_isr_1, CHANGE);

	channelB1Event.Bind(this->quadratureEncoderLeft,&QuadratureEncoder::channelBEventHandler);
	attachInterrupt(this->quadratureEncoderLeft->getPinChannelB(),channel_b_isr_1, CHANGE);

	// encoder 2

	channelA2Event.Bind(this->quadratureEncoderRight,&QuadratureEncoder::channelAEventHandler);
	attachInterrupt(this->quadratureEncoderRight->getPinChannelA(),channel_a_isr_2, CHANGE);

	channelB2Event.Bind(this->quadratureEncoderRight,&QuadratureEncoder::channelBEventHandler);
	attachInterrupt(this->quadratureEncoderRight->getPinChannelB(),channel_b_isr_2, CHANGE);

}

QuadratureEncoder* QuadratureEncoders::left() {
	return quadratureEncoderLeft;
}

QuadratureEncoder* QuadratureEncoders::right() {
	return quadratureEncoderRight;
}

void QuadratureEncoders::reset() {
	this->left()->reset();
}

QuadratureEncoders::~QuadratureEncoders() {
}

signed int QuadratureEncoders::count(QuadratureEncoderSide side) {
	switch(side) {
	  case QuadratureEncoderSide::quadrature_encoder_right:
		return this->right()->count();
	  default:
		return this->left()->count();
	}
}
