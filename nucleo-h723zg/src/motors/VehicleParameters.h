/*
 * VehicleParameters.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_VEHICLEPARAMETERS_H_
#define SRC_VEHICLEPARAMETERS_H_

// Motor parameters — Nucleo Config C: SYSCLK=64 MHz, PSC=0, ARR=3199 → 20 kHz
// ARR=3199 means CCR values run 0–3199; 3199 = 100 % duty cycle.
// PWM_MIN is the motor deadband threshold — re-characterise on hardware.
#define VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY 20000.0F          /* Hz — 64 MHz / 3200 */
#define VEHICLE_MOTOR_DRIVER_PWM_RESOLUTION 11               /* floor(log2(3200)) bits */
#define VEHICLE_MOTOR_DRIVER_CURRENT_MILLIVOLT_PR_AMP 525
#define VEHICLE_MOTOR_DRIVER_PWM_MAX 3199                    /* ARR value = 100 % duty */
#define VEHICLE_MOTOR_DRIVER_PWM_MIN 20                     /* deadband — tune on hardware */

// Quadrature Encoder parameters
#define VEHICLE_MOTOR_ENCODER_COUNT_PR_REV 3200

// general
#define VEHICLE_WHEEL_DIAMETER_CM 9
#define VEHICLE_WHEEL_RADIUS_CM VEHICLE_WHEEL_DIAMETER_CM/2
#define VEHICLE_MIMIMUM_CURRENT_DRAW_MILLI_AMP 10

// PI velocity controller coefficients
// Original Teensy values (13-bit PWM, ARR=8191): B0=0.92, B1=-0.79, A1=-1
// Rescaled for Nucleo 20 kHz config (ARR=3199):
//   scale = 3200 / 8192 = 0.390625
//   Each PWM count on Nucleo carries ~2.56x more voltage than on Teensy, so gains
//   must be divided by the same ratio to preserve the same closed-loop behaviour.
//   B0_new = 0.92  * 0.390625 ≈ 0.3594
//   B1_new = -0.79 * 0.390625 ≈ -0.3086
//   NOTE: These are a first-pass analytical rescaling — verify and re-tune on hardware.
#define VEHICLE_PI_CONTROL_COEFFICIENT_B0  0.3594F
#define VEHICLE_PI_CONTROL_COEFFICIENT_B1 -0.3086F
#define VEHICLE_PI_CONTROL_COEFFICIENT_A1 -1

// P position controller coeffiences
#define VEHICLE_POSITION_CONTROL_COEFFICIENT_P  80

// encoder velocity filter — 1st-order IIR lowpass on delta_us
// Form: H(z) = (B0 + B1·z⁻¹) / (1 + A1·z⁻¹),  DC gain = (B0+B1)/(1+A1) = 1.0
// Coefficients: B0 = B1 = (1−α)/2,  A1 = −α,  where α = exp(−Ts/τ)
//
//  OLD  α = 0.88162, τ ≈  0.8 ms — too fast; single-edge jitter caused lw spikes
//       to 9–10 rad/s in measured telemetry at ref = 7 rad/s.
//  NEW  α = 0.98000, τ ≈  5.0 ms — attenuates jitter spikes by >10×,
//       adds < 2° phase lag at 1 Hz control bandwidth → negligible.
//
// Derivation (Ts = 100 µs):
//   α    = exp(−100e−6 / 5e−3) = exp(−0.02) = 0.98019 ≈ 0.98
//   B0   = B1 = (1 − 0.98) / 2 = 0.01
//   A1   = −0.98
//   DC check: (0.01 + 0.01) / (1 − 0.98) = 0.02 / 0.02 = 1.0 ✓
#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0   0.01F
#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1   0.01F
#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1  -0.98F

//#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0  0.0591907F
//#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1  0.0591907F
//#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1  -0.88161859F

// sample timer
#define VEHICLE_SAMPLE_TIMER_INTERVAL_MICROS 100

// serial
#define VEHICLE_SERIAL_BAUD_RATE 115200

#endif /* SRC_VEHICLEPARAMETERS_H_ */
