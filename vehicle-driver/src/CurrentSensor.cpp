/*
 * CurrentSensor.cpp
 *
 *  Created on: Dec 28, 2022
 *      Author: robot1
 */

#include "CurrentSensor.h"

CurrentSensor::CurrentSensor(uint8_t sensorPin, uint16_t milliVoltPrAmp) {
	this->sensorPin = sensorPin;
	this->milliVoltPrAmp = milliVoltPrAmp;
	pinMode(this->sensorPin, INPUT);
}

float CurrentSensor::getCurrent() {
	int sensorValue = analogRead(this->sensorPin);
	float  mV =  sensorValue * (5.0 / 1023.0)*1000;
	//Serial.print(this->milliVoltPrAmp);
	//Serial.println(" v/amp");
	//Serial.print(mV);
	//Serial.println(" mV");
	float mA = mV/this->milliVoltPrAmp*1000;
	//Serial.print(mA);
	//Serial.println(" mA");
	// Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
	//float  voltage =  sensorValue * (5.0 / 1023.0);
	return mA;
}

CurrentSensor::~CurrentSensor() {
}
