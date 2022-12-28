/*
 * QuadratureEncorderParameters.h
 *
 *  Created on: Sep 21, 2022
 *      Author: robot1
 */

#ifndef SRC_QUADRATUREENCORDERPARAMETERS_H_
#define SRC_QUADRATUREENCORDERPARAMETERS_H_

#include <Arduino.h>
#include <math.h>

class QuadratureEncorderParameters {
public:
	QuadratureEncorderParameters(
			uint8_t pinChannelA,
			uint8_t pinChannelB,
			uint8_t wheelDiameterCm,
			uint16_t countPrRevolution);
	virtual ~QuadratureEncorderParameters();

	int getPinChannelA() const {
			return pinChannelA;
		}

	int getPinChannelB() const {
		return pinChannelB;
	}

    double calculateCmFromCount(signed int count);
    double calculateAngularVelocity(uint32_t timeDiffMs);
    double calculateAngularVelocityFast(uint32_t timeDiffMs);


	double msToSeconds(uint32_t ms);

	double getConstantCountsToRadians() const {
		return constantCountsToRadians;
	}

private:
	int pinChannelA;
	int pinChannelB;

	double constantCountsToCm;
	double constantCountsToRadians;

	double getConstantCountsToCm() const {
		return constantCountsToCm;
	}
};

#endif /* SRC_QUADRATUREENCORDERPARAMETERS_H_ */
