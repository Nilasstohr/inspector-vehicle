/*
 * TestVehicleSystem.h
 *
 *  Created on: Sep 15, 2022
 *      Author: robot1
 */

#ifndef SRC_TESTVEHICLESYSTEM_H_
#define SRC_TESTVEHICLESYSTEM_H_

#include "VehicleTestBase.h"
#include <vector>

class TestVehicleSystem: private VehicleTestBase{
public:
	TestVehicleSystem();
	virtual ~TestVehicleSystem();
	// tests
	void testIncreasedEncoderCounts();
	void testDecreasedEncoderCounts();
	void testCanGetPositionForward();
	void testCanGetPositionBackWards();
	void testCanAngularVelocity();
	void testManualAngularVelocityCalc();
	void testCanDoEncoderRecord();
	void testCanSampleEncoderRecords();
private:
	MotorDrivers * motorDrivers;
	QuadratureEncoders * quadratureEncoders;
	MotorDrivers* motors() const {
			return motorDrivers;
		}
	QuadratureEncoders* encoders() const {
		return quadratureEncoders;
	}
	// sample timer
	IntervalTimer * sampleTimer;
	void sampleEventTimerHandler();

	// helpers
	String *count(signed int &count,int side);
	void resetTest();
	std::vector<EncoderRecord> record;
};

#endif /* SRC_TESTVEHICLESYSTEM_H_ */
