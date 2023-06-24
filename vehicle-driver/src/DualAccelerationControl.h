/*
 * DualAccelerationControl.h
 *
 *  Created on: Jun 21, 2023
 *      Author: robot1
 */

#ifndef SRC_DUALACCELERATIONCONTROL_H_
#define SRC_DUALACCELERATIONCONTROL_H_

#include "Arduino.h"

class DualAccelerationControl {
//  ω² − ω_0² = 2α(ϕ − ϕ_0 ) ⇒ α = − ω_0²/(2*(ϕ − ϕ_0)), ϕ=p/r
public:
	DualAccelerationControl(double wheelRadius);
	void setAlfa(double dStart,double dEnd,double w0);
	double getVelocity(double position);
	virtual ~DualAccelerationControl();
private:
	double alfa;
	double w0;
	double r;
	double fi0;
};

#endif /* SRC_DUALACCELERATIONCONTROL_H_ */

