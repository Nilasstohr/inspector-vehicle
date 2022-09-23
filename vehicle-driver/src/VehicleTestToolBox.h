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
#include "MotorDrivers.h"

class VehicleTestToolBox {
public:
	VehicleTestToolBox();
	QuadratureEncoders * createQuadratureEncoders();
	MotorDrivers * createMotorDrivers();
	virtual ~VehicleTestToolBox();

private:
	QuadratureEncoders * quadratureEncoders;
	MotorDrivers * motorDrivers;
};

#endif /* SRC_VEHICLETESTTOOLBOX_H_ */
