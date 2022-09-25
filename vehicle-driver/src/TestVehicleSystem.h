/*
 * TestVehicleSystem.h
 *
 *  Created on: Sep 15, 2022
 *      Author: robot1
 */

#ifndef SRC_TESTVEHICLESYSTEM_H_
#define SRC_TESTVEHICLESYSTEM_H_

#include "VehicleTestBase.h"

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
private:
	MotorDrivers * motorDrivers;
	QuadratureEncoders * quadratureEncoders;
	MotorDrivers* motors() const {
			return motorDrivers;
		}
	QuadratureEncoders* encoders() const {
		return quadratureEncoders;
	}
	// helpers
	String *count(signed int &count,int side);
};

#endif /* SRC_TESTVEHICLESYSTEM_H_ */
