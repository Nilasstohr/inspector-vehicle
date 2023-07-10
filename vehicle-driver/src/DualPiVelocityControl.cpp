/*
 * PiDualControl.cpp
 *
 *  Created on: Jun 5, 2023
 *      Author: robot1
 */

#include "DualPiVelocityControl.h"

DualPiVelocityControl::DualPiVelocityControl(
		MotorDrivers* motors,
		QuadratureEncoders* sensors,
		PiVelocityControllers*  controlFilters,
		double maximumOutput,
		double minimumOutput){
	this->motors = motors;
	this->sensors = sensors;
	this->controlFilters = controlFilters;
	this->maximumOutput = maximumOutput;
	this->minimumOutput = minimumOutput;
	deltaTLeft=0;
	deltaTRight=0;
	wLeft=0;
	wRight=0;
	sensorReady=false;
}

void DualPiVelocityControl::update(double referenceVelocity,DrivingDirection dir) {
	update( referenceVelocity,referenceVelocity,  dir);
}

void DualPiVelocityControl::update(double referenceVelocityLeft,
		double referenceVelocityRight, DrivingDirection dir) {
	motors->setDir(dir);
	sensorReady = sensors->isSampleReady();
	if(sensorReady){
		deltaTLeft =
				sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left)
							 ->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros_filtered);
		deltaTRight =
				sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right)
									 ->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros_filtered);

		wLeft  = sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left)
				->getParameters()->calculateAngularVelocity(deltaTLeft);


		wRight = sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right)
						->getParameters()->calculateAngularVelocity(deltaTRight);

		// update controller for left
		if(isinf(wLeft)){
			wLeft = 0;
		}
		wLeft = controlFilters->left()->update(referenceVelocityLeft- wLeft);
		if(wLeft < minimumOutput)
			wLeft =minimumOutput;
		else if(wLeft>maximumOutput){
			wLeft = maximumOutput;
		}
		// update controller for right
		if(isinf(wRight)){
			wRight = 0;
		}

		wRight = controlFilters->right()->update(referenceVelocityRight- wRight);
		if(wRight < minimumOutput)
			wRight =minimumOutput;
		else if(wRight>maximumOutput){
			wRight = maximumOutput;
		}

		motors->drive(wLeft, wRight);
		sensors->clearSampleReady();
	}
}


void DualPiVelocityControl::update(uint16_t pwm) {
	    motors->setDir(DrivingDirection::FORWARD);
		sensorReady = sensors->isSampleReady();
		if(sensorReady){
			deltaTLeft =
					sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left)
								 ->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros_filtered);
			deltaTRight =
					sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right)
										 ->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros_filtered);
			motors->drive(pwm, pwm);
			sensors->clearSampleReady();
		}
}

void DualPiVelocityControl::reset() {
	motors->stop();
	sensors->reset();
	controlFilters->left()->reset();
	controlFilters->right()->reset();
}

bool DualPiVelocityControl::wasSensorReady() {
	return sensorReady;
}
DualPiVelocityControl::~DualPiVelocityControl() {
	// TODO Auto-generated destructor stub
}

