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
#include "VehicleDrivingModeTypes.h"

class DualPiVelocityControl {
public:
	DualPiVelocityControl(
			MotorDrivers* motors,
			QuadratureEncoders* sensors,
			PiVelocityControllers*  controlFilters,
			double maximumOutput,
			double minimumOutput);
	void update(double referenceVelocity,DrivingDirection dir);
	void update(uint16_t pwm);
	virtual ~DualPiVelocityControl();

	uint32_t getDeltaTLeft() const {
		return deltaTLeft;
	}

	uint32_t getDeltaTRight() const {
		return deltaTRight;
	}
	bool wasSensorReady();
	void reset();

private:
	MotorDrivers* motors;
	QuadratureEncoders* sensors;
	PiVelocityControllers*  controlFilters;
	double maximumOutput;
	double minimumOutput;
	uint32_t deltaTLeft;
	uint32_t deltaTRight;
	double wLeft;
	double wRight;
	bool sensorReady;
};

#endif /* SRC_DUALPIVELOCITYCONTROL_H_ */
