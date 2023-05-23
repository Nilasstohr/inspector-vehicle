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
			VEHICLE_PIN_QUADRAENCODER_RIGHT_CHANNEL_B,
			VEHICLE_WHEEL_RADIUS_CM,
			VEHICLE_MOTOR_ENCODER_COUNT_PR_REV);
}

MotorDrivers* VehicleTestToolBox::createMotorDrivers()
{
    this->motorDrivers = new MotorDrivers(
		new MotorDriverPins(
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_PWM,
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_INA,
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_INB,
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_FB,
				VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY,
				VEHICLE_MOTOR_DRIVER_CURRENT_MILLIVOLT_PR_AMP
				),
		new MotorDriverPins(
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_PWM,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_INA,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_INB,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_FB,
				VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY,
				VEHICLE_MOTOR_DRIVER_CURRENT_MILLIVOLT_PR_AMP)
    );
    return this->motorDrivers;
}

VehicleTestToolBox::~VehicleTestToolBox() {
	// TODO Auto-generated destructor stub
}
