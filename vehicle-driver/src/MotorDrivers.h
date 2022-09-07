/*
 * MotorDrivers.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_MOTORDRIVERS_H_
#define SRC_MOTORDRIVERS_H_

#include "MotorDriverPins.h"
#include "MotorDriver.h"

class MotorDrivers {
public:
	MotorDrivers(MotorDriverPins *motorDriverPinsLeft,MotorDriverPins *motorDriverPinsRight);
	virtual ~MotorDrivers();
private:
	MotorDriver *motorDriverLeft;
	MotorDriver *motorDriverRight;
};

#endif /* SRC_MOTORDRIVERS_H_ */
