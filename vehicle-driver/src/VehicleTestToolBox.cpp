/*
 * VehicleTestToolBox.cpp
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#include "VehicleTestToolBox.h"

VehicleTestToolBox::VehicleTestToolBox() {
	analogWriteResolution(VEHICLE_MOTOR_DRIVER_PWM_RESOLUTION);
	Serial.begin(VEHICLE_SERIAL_BAUD_RATE);
	// TODO Auto-generated constructor stub

}

QuadratureEncoders* VehicleTestToolBox::createQuadratureEncoders() {


	TransposedIIRFilter * sensorFilterLeft = new TransposedIIRFilter(
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1);

	TransposedIIRFilter * sensorFilterRight = new TransposedIIRFilter(
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1);

	QuadratureEncorderParameters *encorderParametersLeft = new QuadratureEncorderParameters(
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_A,
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_B,
			VEHICLE_WHEEL_RADIUS_CM,
			VEHICLE_MOTOR_ENCODER_COUNT_PR_REV);

	QuadratureEncorderParameters *encorderParametersRight = new QuadratureEncorderParameters(
			VEHICLE_PIN_QUADRAENCODER_RIGHT_CHANNEL_A,
			VEHICLE_PIN_QUADRAENCODER_RIGHT_CHANNEL_B,
			VEHICLE_WHEEL_RADIUS_CM,
			VEHICLE_MOTOR_ENCODER_COUNT_PR_REV);

	return new QuadratureEncoders(
			encorderParametersLeft,
			sensorFilterLeft,
			encorderParametersRight,
			sensorFilterRight,
			VEHICLE_SAMPLE_TIMER_INTERVAL_MICROS);
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
