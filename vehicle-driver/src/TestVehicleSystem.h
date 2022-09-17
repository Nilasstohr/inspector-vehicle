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
	void testIncreasedEncoderCounts();
	void testDecreasedEncoderCounts();
private:
	MotorDrivers * motorDrivers;
	QuadratureEncoders * quadratureEncoders;
	MotorDrivers* motors() const {
			return motorDrivers;
		}
	QuadratureEncoders* encoders() const {
		return quadratureEncoders;
	}
};

#endif /* SRC_TESTVEHICLESYSTEM_H_ */
