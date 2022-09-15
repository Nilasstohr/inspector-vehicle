/*
 * TestRunner.h
 *
 *  Created on: Aug 18, 2022
 *      Author: robot1
 */

#ifndef SRC_VEHICLETESTRUNNER_H_
#define SRC_VEHICLETESTRUNNER_H_

#include "TestQuadratureEncorder.h"
#include "TestMotorDrivers.h"
#include "TestVehicleSystem.h"
#include "Logger.h"


class VehicleTestRunner {
public:
	VehicleTestRunner();
	virtual ~VehicleTestRunner();
};

#endif /* SRC_VEHICLETESTRUNNER_H_ */
