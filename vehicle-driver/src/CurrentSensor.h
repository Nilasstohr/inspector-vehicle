/*
 * CurrentSensor.h
 *
 *  Created on: Dec 28, 2022
 *      Author: robot1
 */

#ifndef SRC_CURRENTSENSOR_H_
#define SRC_CURRENTSENSOR_H_

#include <Arduino.h>
#include "Logger.h"

class CurrentSensor {
public:
	CurrentSensor(uint8_t sensorPin, uint16_t milliVoltPrAmp);
	float getCurrent();
	virtual ~CurrentSensor();
private:
	uint8_t sensorPin;
    uint16_t milliVoltPrAmp;
};

#endif /* SRC_CURRENTSENSOR_H_ */
