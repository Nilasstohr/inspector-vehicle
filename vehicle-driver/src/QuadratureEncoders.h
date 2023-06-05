/*
 * QuadratureEncoders.h
 *
 *  Created on: Sep 4, 2022
 *      Author: robot1
 */

#ifndef SRC_QUADRATUREENCODERS_H_
#define SRC_QUADRATUREENCODERS_H_

#include "QuadratureEncoder.h"
#include "QuadratureEncorderParameters.h"
#include "TransposedIIRFilter.h"

class QuadratureEncoders {
public:
	enum QuadratureEncoderSide
	{
	    quadrature_encoder_left,
		quadrature_encoder_right
	};

	QuadratureEncoders(
			QuadratureEncorderParameters * paramsLeft,
			TransposedIIRFilter * sensorFilterLeft,
			QuadratureEncorderParameters * paramsRight,
			TransposedIIRFilter * sensorFilterRight,
			unsigned int timerIntervalUs);
	void setupEncoders();
	virtual ~QuadratureEncoders();
	QuadratureEncoder * left();
	QuadratureEncoder * right();
	void reset();
	signed int count(QuadratureEncoderSide side);
	float velocity(QuadratureEncoderSide side);
	uint32_t timerInterval(QuadratureEncoderSide side);
	double position(QuadratureEncoderSide side);
	String * count(signed int & count,QuadratureEncoderSide side);
	QuadratureEncoder * encoder(QuadratureEncoderSide side);
	bool isSampleReady();
	void clearSampleReady();

private:
	QuadratureEncoder * quadratureEncoderLeft;
	QuadratureEncoder * quadratureEncoderRight;

	IntervalTimer * sampleTimer;
	bool sampleReady;
	unsigned int timerIntervalUs;
	void sampleEventTimerHandler();
};

#endif /* SRC_QUADRATUREENCODERS_H_ */
