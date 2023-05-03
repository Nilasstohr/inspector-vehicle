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
	void canSetMotorDriverPins();
	void canSetupMotorDriver();
	void canSetupMotorDrivers();
	void canDriveForward();
	void canDriveReverse();
	void canReadCurrentSenseWhenStopped();
	void canReadCurrentSenseWhenStarted();
	void canDetermineLowestPWM();
	//
	virtual ~TestMotorDrivers();
private:
	void printMotorSettings(MotorDriverPins * motorDriverPins);
	float getCurrent(MotorDrivers *motorDrivers,Side side);
	void checkCurrentOutputByState(MotorDrivers *motorDrivers,bool state);
	void checkCurrentOutput(float current,bool state);
};


#endif /* SRC_TESTMOTORDRIVERS_H_ */
