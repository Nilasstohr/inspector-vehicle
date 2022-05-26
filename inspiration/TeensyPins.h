/*
 * TeensyPins.h
 *
 *  Created on: 30/05/2015
 *      Author: Nilasstohr
 */

#ifndef TEENSYPINS_H_
#define TEENSYPINS_H_

#define PWM_FREQ 300
#define TS 1/600

#define SPEED_CONTROLLER_PWM_FREQ 300 // Hz
#define SPEED_CONTROLLER_PWM_RES 16  // 16 bit

#define PIN_RX1 0
#define PIN_TX1 1
#define PIN_CS2 2

#define PIN_PWM_20 20
#define PIN_PWM_21 21
#define PIN_PWM_22 22
#define PIN_PWM_23 23

#define LSM9DS0_CSG  9  // CSG connected to Arduino pin 9
#define LSM9DS0_CSXM 10 // CSXM connected to Arduino pin 10

#define PIN_DIGITAL_14 14

#define LOG_ITEM_CAP 5
#define LOG_ITEM_CAP_RAW 5
#define LOG_ITEM_CAP_CONTROL 1000

// Distance sensor
#define  DIS_SENSOR_A_EX_FIT  4.947
#define  DIS_SERNSR_B_EX_FIX -0.03511
#define  DIS_SENSOR_ADC_RES 16
#define  DIS_SENSOR_ADC_VOLTAGE_SCALE 5.0/65536
#define  DIS_SENSOR_AVG_NUM 15
#define  DIS_SENSOR_PIN 16

#define FORCE_TORQUE_CS 10
#define FORCE_TORQUE_CLKPIN 14
#define FORCE_TORQUE_MISOPIN 12
#define FORCE_TORQUE_RESORLUTION  0.000488 //mm/discrete
#define FORCE_TORQUE_KSPRING   494 //N/m
#define FORCE_TORQUE_MOVING_AVERAGE_SIZE 10

#define RPM_AVERAGE_NUM 20
#define RPM_ANALOG_PIN 23

#define LSM9DS1_M_CS	10 // Can be any digital pin
#define LSM9DS1_AG_CS	9  // Can be any other digital pin

#endif /* TEENSYPINS_H_ */
