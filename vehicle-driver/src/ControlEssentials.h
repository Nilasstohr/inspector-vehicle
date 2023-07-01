/*
 * ControlEssentials.h
 *
 *  Created on: Jun 27, 2023
 *      Author: robot1
 */

#ifndef CONTROLESSENTIALS_H_
#define CONTROLESSENTIALS_H_

#include "MotorDrivers.h"
#include "QuadratureEncoders.h"
#include "VehicleDrivingModeTypes.h"

class ControlEssentials {
public:
	ControlEssentials(
				MotorDrivers* motors,
				QuadratureEncoders* sensors,
				double maximumOutput,
				double minimumOutput);
	MotorDrivers* getMotors();
	QuadratureEncoders* getSensors();
	double getMaximumOutput();
	double getMinimumOutput();
	double checkForSaturation(double input);
	double checkForSaturation(double input, double minimumOutput);
	virtual ~ControlEssentials();

	double getWLeft() const {
		return wLeft;
	}

	void setWLeft(double wLeft) {
		this->wLeft = wLeft;
	}

	double getWRight() const {
		return wRight;
	}

	void setWRight(double wRight) {
		this->wRight = wRight;
	}
	void reset();

private:
	MotorDrivers* motors;
	QuadratureEncoders* sensors;
	double checkForSaturation(double input,double maximumOutput,
			double minimumOutput);
	double maximumOutput;
	double minimumOutput;
	double wLeft;
	double wRight;

};

#endif /* CONTROLESSENTIALS_H_ */
