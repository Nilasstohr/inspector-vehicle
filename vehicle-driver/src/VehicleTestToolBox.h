/*
 * VehicleTestToolBox.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_VEHICLETESTTOOLBOX_H_
#define SRC_VEHICLETESTTOOLBOX_H_

#include "Logger.h"
#include "VehiclePins.h"
#include "VehicleParameters.h"
#include "QuadratureEncoders.h"
#include "QuadratureEncoderTypes.h"
#include "MotorDrivers.h"
#include "EncoderRecord.h"
#include "DualPiVelocityControl.h"

class VehicleTestToolBox {
public:
	VehicleTestToolBox();
	QuadratureEncoders * buildQuadratureEncoders();
	MotorDrivers * buildMotorDrivers();
	DualPiVelocityControl* buildDualPiVelocityControl();
	MotorDrivers * getMotorsDrivers();
	virtual ~VehicleTestToolBox();

private:
	QuadratureEncoders * quadratureEncoders;
	MotorDrivers * motorDrivers;
	DualPiVelocityControl *dualPiVelocityControl;
};

#endif /* SRC_VEHICLETESTTOOLBOX_H_ */
