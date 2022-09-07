/*
 * MotorDriver.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_MOTORDRIVER_H_
#define SRC_MOTORDRIVER_H_

#include "MotorDriverPins.h"

class MotorDriver {
public:
	MotorDriver(MotorDriverPins * motorDriverPins);
	virtual ~MotorDriver();

private:
	MotorDriverPins * motorDriverPins;
};

#endif /* SRC_MOTORDRIVER_H_ */
