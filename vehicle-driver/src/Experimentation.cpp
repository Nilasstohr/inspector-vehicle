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

#define M1_INV_D2 35
#define M1_IN1 36
#define M1_IN2 37

#define ENCODER_CH_A_M1 22
#define ENCODER_CH_B_M1 23

#define ENCODER_CH_A_M2 31
#define ENCODER_CH_B_M2 32

#define LED 13

Experimentation::Experimentation()
{
	Serial.begin(115200);
	pinMode(LED, OUTPUT);

	this->quadratureEncoderM2 = new QuadratureEncoder(ENCODER_CH_A_M2,ENCODER_CH_B_M2);
	this->quadratureEncoderM2->setupChannels();

	this->quadratureEncoderM1 = new QuadratureEncoder(ENCODER_CH_A_M1,ENCODER_CH_B_M1);
	//this->quadratureEncoderM1->setupChannels();

	delay(1000);

	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "Running teensy experimentaion");

	// setting pwm
	Logger::verbose(__FUNCTION__, "setting analog resolution to 12 bit");
	analogWriteResolution(12);

	//---------------------------M1

	Logger::verbose(__FUNCTION__, "setting M1 inv D2 pwm frequency to 11718.75 Hz");
	analogWriteFrequency(M1_INV_D2, 11718.75);


	Logger::verbose(__FUNCTION__, "setting M1 IN1 to output and high");
	pinMode(M1_IN1, OUTPUT);
	digitalWrite(M1_IN1, HIGH);

	Logger::verbose(__FUNCTION__, "setting M1 IN2 to output and low");
	pinMode(M1_IN2, OUTPUT);
	digitalWrite(M1_IN2, LOW);

	Logger::verbose(__FUNCTION__, "setting M1 inv D2 output to 50% duty");
	analogWrite(M1_INV_D2,4048);

	//---------------------------M2

	Logger::verbose(__FUNCTION__, "setting M2 inv D2 pwm frequency to 11718.75 Hz");
	analogWriteFrequency(M2_INV_D2, 11718.75);

	Logger::verbose(__FUNCTION__, "setting M2 IN1 to output and high");
	pinMode(M2_IN1, OUTPUT);
	digitalWrite(M2_IN1, HIGH);

	Logger::verbose(__FUNCTION__, "setting M2 IN2 to output and low");
	pinMode(M2_IN2, OUTPUT);
	digitalWrite(M2_IN2, LOW);

	Logger::verbose(__FUNCTION__, "setting M2 inv D2 output to 50% duty");
	analogWrite(M2_INV_D2,4048);

    char buffer[40];
	while(true){
		digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
		delay(10);               // wait for a second
		digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
		delay(10);

		//Serial.println("m1: ");
		//Serial.print(this->quadratureEncoderM1->getCounts());
		Serial.print(" m2: ");
		Serial.println(this->quadratureEncoderM2->getCounts());

    }
}

Experimentation::~Experimentation()
{

}

