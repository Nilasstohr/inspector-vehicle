/*
 * VehicleTestToolBox.cpp
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#include "VehicleTestToolBox.h"

VehicleTestToolBox::VehicleTestToolBox() {
	// TODO Auto-generated constructor stub

}

QuadratureEncoders* VehicleTestToolBox::createQuadratureEncoders() {
	return new QuadratureEncoders(
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_A,
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_B,
			VEHICLE_PIN_QUADRAENCODER_RIGHT_CHANNEL_A,
			VEHICLE_PIN_QUADRAENCODER_RIGHT_CHANNEL_B);
}

VehicleTestToolBox::~VehicleTestToolBox() {
	// TODO Auto-generated destructor stub
}

MotorDrivers* VehicleTestToolBox::createMotorDrivers()
{
	return NULL;
	/*
    this->motorDrivers = new MotorDrivers(
		new MotorDriverPins(
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_D2,
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN1,
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN2,
				VEHICLE_MOTOR_DRIVER_PWM_RESOLUTION
				),
		new MotorDriverPins(
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_D2,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_IN1,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_IN2,
				VEHICLE_MOTOR_DRIVER_PWM_RESOLUTION)
    );
    return this->motorDrivers;
    */
}
