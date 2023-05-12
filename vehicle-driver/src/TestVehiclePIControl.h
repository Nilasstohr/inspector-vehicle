/*
 * TestVehiclePIControl.h
 *
 *  Created on: Dec 30, 2022
 *      Author: robot1
 */

#ifndef SRC_TESTVEHICLEPICONTROL_H_
#define SRC_TESTVEHICLEPICONTROL_H_

#include "VehicleTestBase.h"
#include "AverageCalc.h"
#include "FirstOrderFilter.h"

class TestVehiclePIControl: private VehicleTestBase{
public:
	TestVehiclePIControl();
	void canInitializePiController();
	void canPerformPiControl();
	virtual ~TestVehiclePIControl();
private:
	MotorDrivers * motorDrivers;
	QuadratureEncoders * quadratureEncoders;
	FirstOrderFilter * piControllerLeft;
	FirstOrderFilter * piControllerRight;
	MotorDrivers* motors() const {
			return motorDrivers;
		}
	QuadratureEncoders* encoders() const {
		return quadratureEncoders;
	}
	// sample timer
	IntervalTimer * sampleTimer;
	AverageCalc * averageCalcLeft;
	AverageCalc * averageCalcRight;
	uint8_t averageSize;
	uint8_t getAverageSize();
	void sampleEventTimerHandler();
	volatile uint32_t intervalLeft;
	volatile uint32_t intervalRight;
	volatile bool ready= false;

	volatile double timeSeconds;
	volatile double timeMillis;

	double getAngularVelocity(uint32_t interval);
	uint32_t getTimeInterval(QuadratureEncoders::QuadratureEncoderSide side);
	AverageCalc * getAverageCalcLeft();
	AverageCalc*  getAverageCalcRight();
	double getTimeInSeconds();
	unsigned int timerIntervalUs;
	double timerIntervalMillis;
	double timerIntervalSeconnds;
	unsigned int getTimerIntervalUs();
	double getTimeIntervalSeconds();
	double getTimerIntervalMillis();
	double getTimeInMillis();
};

#endif /* SRC_TESTVEHICLEPICONTROL_H_ */
