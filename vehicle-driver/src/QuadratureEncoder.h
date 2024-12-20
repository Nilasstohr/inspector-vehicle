/*
 * QuadratureEncoder.h
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#ifndef SRC_QUADRATUREENCODER_H_
#define SRC_QUADRATUREENCODER_H_

#include <Arduino.h>
#include "SyncEvent.h"
#include "QuadratureEncoderInterval.h"
#include "math.h"
#include "QuadratureEncorderParameters.h"
#include "QuadratureEncoderTypes.h"
#include "TransposedIIRFilter.h"

class QuadratureEncoder {
public:


	QuadratureEncoder(QuadratureEncorderParameters *parameters,TransposedIIRFilter *filter);
	virtual ~QuadratureEncoder();

	signed int count();

	uint32_t getTimeInterval();

	void updateVelocity();
	void updatePosition();
	uint16_t countInterval();

	void setupChannels();

	void channelAEventHandler();
    void channelBEventHandler();

	void reset();

    QuadratureEncorderParameters* getParameters() const {
		return parameters;
	}

    double getPosition();

	double getAngularVelocity();

	template<typename ReadValue>
	ReadValue read(QuadratureEncoderReadTypes readType);

private:

	QuadratureEncoderInterval *timeIntervalHandler;
	QuadratureEncoderInterval *countIntervalHandler;
	QuadratureEncorderParameters *parameters;
	TransposedIIRFilter *filter;
    double position;

    QuadratureEncoderInterval * getTimeIntervalHandler() {
		return timeIntervalHandler;
	}

    void setPosition(double position) {
		this->position = position;
	}

    uint32_t getTimeIntervalFiltered();

    signed int counts =0;
    uint32_t timeMicros;

	void refresh();
};

template<typename ReadValue>
inline ReadValue QuadratureEncoder::read(QuadratureEncoderReadTypes readType) {
	switch(readType){
		case QuadratureEncoderReadTypes::position_linear:{
			return this->getPosition();
		}
		case QuadratureEncoderReadTypes::angular_velocity_radian_pr_sec:{
			return this->getAngularVelocity();
		}
		case QuadratureEncoderReadTypes::encoder_counts:{
			return this->count();
		}
		case QuadratureEncoderReadTypes::time_interval_micros:{
			return this->getTimeInterval();
		}
		case QuadratureEncoderReadTypes::time_interval_micros_filtered:{
			return this->getTimeIntervalFiltered();
		}

	}
}

#endif /* SRC_QUADRATUREENCODER_H_ */
