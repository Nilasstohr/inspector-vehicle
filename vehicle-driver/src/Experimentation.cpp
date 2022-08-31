/*
 * Experimentation.cpp
 *
 *  Created on: Aug 28, 2022
 *      Author: robot1
 */

#include "Experimentation.h"

#define M2_INV_D2 2
#define M2_IN1 3
#define M2_IN2 4
#define EN 5
#define SLEW 6

#define ENCODER_CH_A 0
#define ENCODER_CH_B 1

#define LED 13

Experimentation::Experimentation()
{
	Serial.begin(115200);
	pinMode(LED, OUTPUT);

	this->quadratureEncoder = new QuadratureEncoder(ENCODER_CH_A,ENCODER_CH_B);
	this->quadratureEncoder->setupChannels();

	delay(1000);

	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "Running teensy experimentaion");

	// setting pwm

	Logger::verbose(__FUNCTION__, "setting analog resolution to 12 bit");

	analogWriteResolution(12);

	Logger::verbose(__FUNCTION__, "setting M2 inv D2 pwm frequency to 11718.75 Hz");

	analogWriteFrequency(M2_INV_D2, 11718.75);

	Logger::verbose(__FUNCTION__, "setting SLEW to output and low (for pwn < 11kHz)");

	pinMode(SLEW, OUTPUT);
    digitalWrite(SLEW, LOW);

    //

	Logger::verbose(__FUNCTION__, "setting EN to output and high");

	pinMode(EN, OUTPUT);
    digitalWrite(EN, HIGH);

	Logger::verbose(__FUNCTION__, "setting M2 IN1 to output and high");

	pinMode(M2_IN1, OUTPUT);
	digitalWrite(M2_IN1, HIGH);

	Logger::verbose(__FUNCTION__, "setting M2 IN2 to output and low");

	pinMode(M2_IN2, OUTPUT);
	digitalWrite(M2_IN2, LOW);

	Logger::verbose(__FUNCTION__, "setting M2 inv D2 output to 50% duty");

	analogWrite(M2_INV_D2,2048);

	while(true){
		digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
		delay(10);               // wait for a second
		digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
		delay(10);
		Serial.println(this->quadratureEncoder->getCounts());
    }
}

Experimentation::~Experimentation()
{

}

