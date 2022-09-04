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
	this->quadratureEncoder1 = new QuadratureEncoder(channel1A,channel1B);
	this->quadratureEncoder2 = new QuadratureEncoder(channel2A,channel2B);
}

void QuadratureEncoders::setupEncoders() {

	// encoder 1

	channelA1Event.Bind(this->quadratureEncoder1,&QuadratureEncoder::channelAEventHandler);
	attachInterrupt(this->quadratureEncoder1->getPinChannelA(),channel_a_isr_1, CHANGE);

	channelB1Event.Bind(this->quadratureEncoder1,&QuadratureEncoder::channelBEventHandler);
	attachInterrupt(this->quadratureEncoder1->getPinChannelB(),channel_b_isr_1, CHANGE);

	// encoder 2

	channelA2Event.Bind(this->quadratureEncoder2,&QuadratureEncoder::channelAEventHandler);
	attachInterrupt(this->quadratureEncoder2->getPinChannelA(),channel_a_isr_2, CHANGE);

	channelB2Event.Bind(this->quadratureEncoder2,&QuadratureEncoder::channelBEventHandler);
	attachInterrupt(this->quadratureEncoder2->getPinChannelB(),channel_b_isr_2, CHANGE);

}

QuadratureEncoder* QuadratureEncoders::getQuadratureEncoder1() {
	return quadratureEncoder1;
}

QuadratureEncoder* QuadratureEncoders::getQuadratureEncoder2() {
	return quadratureEncoder2;
}
QuadratureEncoders::~QuadratureEncoders() {
	// TODO Auto-generated destructor stub
}


