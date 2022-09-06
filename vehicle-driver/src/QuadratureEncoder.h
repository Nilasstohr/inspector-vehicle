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

class QuadratureEncoder {
public:
	QuadratureEncoder(int pinChannelA,int pinChannelB);
	virtual ~QuadratureEncoder();

	signed int getCount();

	void setupChannels();

	int getPinChannelA() const {
		return pinChannelA;
	}

	int getPinChannelB() const {
		return pinChannelB;
	}

	void channelAEventHandler();
    void channelBEventHandler();

	uint16_t getCountInterval() {
		return countInterval;
	}


private:

	int pinChannelA;
	int pinChannelB;

	signed int counts =0;
	uint64_t lastTimeStampMicros =0;
	uint16_t countInterval=0;

	uint64_t getLastTimeStampMicros() {
			return lastTimeStampMicros;
	}

	void setLastTimeStampMicros(uint64_t timeStampMicros) {
		this->lastTimeStampMicros = timeStampMicros;
	}

	void setCountInterval(uint16_t countInterval) {
			this->countInterval = countInterval;
	}

	void updateCountInterval(uint64_t micros);
};

#endif /* SRC_QUADRATUREENCODER_H_ */
