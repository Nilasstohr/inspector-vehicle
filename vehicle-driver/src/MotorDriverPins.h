/*
 * MotorDriverPins.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_MOTORDRIVERPINS_H_
#define SRC_MOTORDRIVERPINS_H_

class MotorDriverPins {
public:
	MotorDriverPins(int pwmD2,int in1,int in2){
		this->pwmD2 = pwmD2;
		this->in1=in1;
		this->in2=in2;
	}
	virtual ~MotorDriverPins();

	int getIn1() const {
		return in1;
	}

	int getIn2() const {
		return in2;
	}

	int getPwmD2() const {
		return pwmD2;
	}

private:
	int pwmD2;
	int in1;
	int in2;
};

#endif /* SRC_MOTORDRIVERPINS_H_ */
