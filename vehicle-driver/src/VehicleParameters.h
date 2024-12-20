/*
 * VehicleParameters.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_VEHICLEPARAMETERS_H_
#define SRC_VEHICLEPARAMETERS_H_

// Motor parameters
#define VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY 18310.55
#define VEHICLE_MOTOR_DRIVER_PWM_RESOLUTION 13
#define VEHICLE_MOTOR_DRIVER_CURRENT_MILLIVOLT_PR_AMP 525
#define VEHICLE_MOTOR_DRIVER_PWM_MAX (int)pow(2,VEHICLE_MOTOR_DRIVER_PWM_RESOLUTION)
#define VEHICLE_MOTOR_DRIVER_PWM_MIN 500

// Quadrature Encoder parameters
//#define VEHICLE_MOTOR_ENCODER_COUNT_PR_REV 9600
#define VEHICLE_MOTOR_ENCODER_COUNT_PR_REV 3200


// general
#define VEHICLE_WHEEL_DIAMETER_CM 9
#define VEHICLE_WHEEL_RADIUS_CM VEHICLE_WHEEL_DIAMETER_CM/2
#define VEHICLE_MIMIMUM_CURRENT_DRAW_MILLI_AMP 10

// PI velocity controller coeffiences
#define VEHICLE_PI_CONTROL_COEFFICIENT_B0  0.92
#define VEHICLE_PI_CONTROL_COEFFICIENT_B1 -0.79
#define VEHICLE_PI_CONTROL_COEFFICIENT_A1 -1

// P position controller coeffiences
#define VEHICLE_POSITION_CONTROL_COEFFICIENT_P  80


// encoder filter
#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0  0.0591907
#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1  0.0591907
#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1  -0.88161859

// sample timer
#define VEHICLE_SAMPLE_TIMER_INTERVAL_MICROS 100

// serial
#define VEHICLE_SERIAL_BAUD_RATE 115200
#define VEHICLE_SERIAL_BAUD_RATE_BT 38400

// manual driving commands
#define VEHICLE_DRIVING_MODE_MANUAL_FORWARD String("FORWARD")
#define VEHICLE_DRIVING_MODE_MANUAL_BACKWARD "BACKWARD"
#define VEHICLE_DRIVING_MODE_MANUAL_TURN_LEFT "TURN_LEFT"
#define VEHICLE_DRIVING_MODE_MANUAL_TURN_RIGHT "TURN_RIGHT"
#define VEHICLE_DRIVING_MODE_MANUAL_STOP "STOP"



#endif /* SRC_VEHICLEPARAMETERS_H_ */
