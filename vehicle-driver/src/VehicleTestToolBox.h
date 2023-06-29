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
#include "DualPPositionControl.h"
#include "ControlEssentials.h"

class VehicleTestToolBox {
public:
	VehicleTestToolBox();
	QuadratureEncoders * buildQuadratureEncoders();
	MotorDrivers * buildMotorDrivers();
	ControlEssentials * buildControlEssentials();
	DualPiVelocityControl* buildDualPiVelocityControl();
	DualPPositionControl * buildPositionControl();
	MotorDrivers * getMotorsDrivers();
	QuadratureEncoders * getSensors();
	virtual ~VehicleTestToolBox();

private:
	QuadratureEncoders * quadratureEncoders;
	MotorDrivers * motorDrivers;
	DualPiVelocityControl *dualPiVelocityControl;
	DualPPositionControl* dualPsitionControl;
	ControlEssentials * controlEssentials;
};

#endif /* SRC_VEHICLETESTTOOLBOX_H_ */
