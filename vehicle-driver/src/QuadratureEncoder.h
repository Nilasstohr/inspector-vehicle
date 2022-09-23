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


private:

	QuadratureEncoderInterval *_timeInterval;
	QuadratureEncoderInterval *_countInterval;
	QuadratureEncorderParameters *parameters;
	double position;

	QuadratureEncoderInterval * getPositionInterval() {
		return _countInterval;
	}

    QuadratureEncoderInterval * getTimeInterval() {
		return _timeInterval;
	}

    void setPosition(double position) {
		this->position = position;
	}


    signed int counts =0;

	void refresh();
};

#endif /* SRC_QUADRATUREENCODER_H_ */
