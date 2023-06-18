/*
 * SerialInterface.h
 *
 *  Created on: Jun 18, 2023
 *      Author: robot1
 */

#ifndef SRC_SERIALINTERFACE_H_
#define SRC_SERIALINTERFACE_H_

#include <Arduino.h>

class SerialInterface {
public:
	SerialInterface();
	bool availableRead();
	String* getReceived();
	virtual ~SerialInterface();
private:
	uint8_t readSize;
    char  buffer[256];
    void printReceived();
};

#endif /* SRC_SERIALINTERFACE_H_ */
