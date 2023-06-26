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
#include "Side.h"
#include "VehicleDrivingModeTypes.h"

class MotorDrivers {
public:
	MotorDrivers(MotorDriverPins *motorDriverPinsLeft,MotorDriverPins *motorDriverPinsRight);
	virtual ~MotorDrivers();
    void forward(uint16_t pwm);
    void forward(uint16_t pwmLeft,uint16_t pwmRight);
    void reverse(uint16_t pwm);
    void setDir(DrivingDirection dir);
    void drive(uint16_t pwmLeft,uint16_t pwmRight);
    void stop();
    float getCurrent(Side side);

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
	void setTurnLeft();
	void setTurnRight();

};

#endif /* SRC_MOTORDRIVERS_H_ */
