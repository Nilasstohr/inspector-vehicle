/*
 * TestMotorDrivers.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_TESTMOTORDRIVERS_H_
#define SRC_TESTMOTORDRIVERS_H_

#include "VehicleTestBase.h"
#include "MotorDriver.h"

class TestMotorDrivers: private VehicleTestBase {
public:
	TestMotorDrivers();
	void canSetupMotorDriver();
	virtual ~TestMotorDrivers();
private:
	MotorDriver * motorDriverLeft;
};


#endif /* SRC_TESTMOTORDRIVERS_H_ */
