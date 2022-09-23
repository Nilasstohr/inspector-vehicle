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

QuadratureEncoders::QuadratureEncoders(
		int channel1A,
		int channel1B,
		int channel2A,
		int channel2B,
		uint8_t wheelDiameterCm,
		uint16_t countPrRevolution)
{
	this->quadratureEncoderLeft = new QuadratureEncoder(
			new QuadratureEncorderParameters(channel1A,channel1B,wheelDiameterCm,countPrRevolution));
	this->quadratureEncoderRight = new QuadratureEncoder(
			new QuadratureEncorderParameters(channel2A,channel2B,wheelDiameterCm,countPrRevolution));
}

void QuadratureEncoders::setupEncoders() {

	// encoder 1

	channelA1Event.Bind(this->quadratureEncoderLeft,&QuadratureEncoder::channelAEventHandler);
	attachInterrupt(this->quadratureEncoderLeft->getParameters()->getPinChannelA(),channel_a_isr_1, CHANGE);

	channelB1Event.Bind(this->quadratureEncoderLeft,&QuadratureEncoder::channelBEventHandler);
	attachInterrupt(this->quadratureEncoderLeft->getParameters()->getPinChannelB(),channel_b_isr_1, CHANGE);

	// encoder 2

	channelA2Event.Bind(this->quadratureEncoderRight,&QuadratureEncoder::channelAEventHandler);
	attachInterrupt(this->quadratureEncoderRight->getParameters()->getPinChannelA(),channel_a_isr_2, CHANGE);

	channelB2Event.Bind(this->quadratureEncoderRight,&QuadratureEncoder::channelBEventHandler);
	attachInterrupt(this->quadratureEncoderRight->getParameters()->getPinChannelB(),channel_b_isr_2, CHANGE);

}

QuadratureEncoder* QuadratureEncoders::left() {
	return quadratureEncoderLeft;
}

QuadratureEncoder* QuadratureEncoders::right() {
	return quadratureEncoderRight;
}

void QuadratureEncoders::reset() {
	this->left()->reset();
	this->right()->reset();
}

signed int QuadratureEncoders::count(QuadratureEncoderSide side) {
	return this->encoder(side)->count();
}

float QuadratureEncoders::velocity(QuadratureEncoderSide side) {
	return NULL;
}

double QuadratureEncoders::position(QuadratureEncoderSide side) {
	return this->encoder(side)->getPosition();
}


String* QuadratureEncoders::count(signed int &count,
		QuadratureEncoderSide side) {
	count = this->count(side);
	String * str = new String();
	switch(side) {
	  case QuadratureEncoderSide::quadrature_encoder_right:
		str->append("right");
		break;
	  default:
		str->append("left");
	}
	str->append(" encoder count was ").append(count);
	return str;
}

QuadratureEncoder* QuadratureEncoders::encoder(QuadratureEncoderSide side) {
	switch(side) {
	  case QuadratureEncoderSide::quadrature_encoder_right:
		return this->right();
	  default:
		return this->left();
	}
}

QuadratureEncoders::~QuadratureEncoders() {
}
