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
    void forward(uint16_t pwm);
    void reverse(uint16_t pwm);
    void stop();

private:
	MotorDriver *motorDriverLeft;
	MotorDriver *motorDriverRight;
    MotorDriver* left() const {
		return motorDriverLeft;
	}
	MotorDriver* right() const {
		return motorDriverRight;
	}
	void setForward();
	void setReverse();
};

#endif /* SRC_MOTORDRIVERS_H_ */
