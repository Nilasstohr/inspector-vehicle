/*
 * TestVehiclePIControl.h
 *
 *  Created on: Dec 30, 2022
 *      Author: robot1
 */

#ifndef SRC_TESTVEHICLEPICONTROL_H_
#define SRC_TESTVEHICLEPICONTROL_H_

#include "VehicleTestBase.h"
#include "PiController.h"

class TestVehiclePIControl: private VehicleTestBase{
public:
	TestVehiclePIControl();
	void canInitializePiController();
	void canPerformPiControl();
	virtual ~TestVehiclePIControl();
private:
	MotorDrivers * motorDrivers;
	QuadratureEncoders * quadratureEncoders;
	PiController * piController;
	MotorDrivers* motors() const {
			return motorDrivers;
		}
	QuadratureEncoders* encoders() const {
		return quadratureEncoders;
	}
	// sample timer
	IntervalTimer * sampleTimer;
	void sampleEventTimerHandler();
	volatile uint32_t omega;
	volatile bool ready= false;
};

#endif /* SRC_TESTVEHICLEPICONTROL_H_ */
