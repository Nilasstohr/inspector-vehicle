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
private:

	int pinChannelA;
	int pinChannelB;

	int getPinChannelA() const {
		return pinChannelA;
	}

	int getPinChannelB() const {
		return pinChannelB;
	}

	void setupChannels();
    void channelAEventHandler();
    void channelBEventHandler();
};

#endif /* SRC_QUADRATUREENCODER_H_ */
