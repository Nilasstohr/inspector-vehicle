/*
 * PiDualControl.h
 *
 *  Created on: Jun 5, 2023
 *      Author: robot1
 */

#ifndef SRC_DUALPIVELOCITYCONTROL_H_
#define SRC_DUALPIVELOCITYCONTROL_H_

#include "MotorDrivers.h"
#include "QuadratureEncoders.h"
#include "PiVelocityControllers.h"

class DualPiVelocityControl {
public:
	DualPiVelocityControl(
			MotorDrivers* motors,
			QuadratureEncoders* sensors,
			PiVelocityControllers*  controlFilters);
	virtual ~DualPiVelocityControl();
private:
	MotorDrivers* motors;
	QuadratureEncoders* sensors;
	PiVelocityControllers*  controlFilters;
};

#endif /* SRC_DUALPIVELOCITYCONTROL_H_ */
