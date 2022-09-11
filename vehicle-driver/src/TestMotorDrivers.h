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
	// tests
	void canSetupMotorDriver();
	void canSetMotorDriverPins();
	//
	virtual ~TestMotorDrivers();
private:
	void printMotorSettings(MotorDriverPins * motorDriverPins);
};


#endif /* SRC_TESTMOTORDRIVERS_H_ */
