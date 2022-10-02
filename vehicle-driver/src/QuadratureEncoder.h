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

class QuadratureEncoder {
public:


	QuadratureEncoder(QuadratureEncorderParameters *parameters);
	virtual ~QuadratureEncoder();

	signed int count();
	float velocity();

	uint16_t timeInterval();
	uint16_t countInterval();

	void setupChannels();

	void channelAEventHandler();
    void channelBEventHandler();

	void reset();

    QuadratureEncorderParameters* getParameters() const {
		return parameters;
	}

    double getPosition() {
    	return this->position;
    }

	double getAngularVelocity() const {
		return angularVelocity;
	}

	template<typename ReadValue>
	ReadValue read(QuadratureEncoderReadTypes readType);

private:

	QuadratureEncoderInterval *_timeInterval;
	QuadratureEncoderInterval *_countInterval;
	QuadratureEncorderParameters *parameters;
    double position;
    double angularVelocity;

    QuadratureEncoderInterval * getTimeInterval() {
		return _timeInterval;
	}

    void setPosition(double position) {
		this->position = position;
	}
    void setAngularVelocity(double angularVelocity) {
		this->angularVelocity = angularVelocity;
}

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
		case QuadratureEncoderReadTypes::time_ms:{
			return this->timeMicros;
		}
		case QuadratureEncoderReadTypes::time_interval_micros:{
			return this->getTimeInterval()->get();
		}
	}
}

#endif /* SRC_QUADRATUREENCODER_H_ */
