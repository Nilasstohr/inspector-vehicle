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
	bool hasMessage();
	String* getMessage();
	bool validateCommand(uint8_t size, char id);
	void sendAck();
	virtual ~SerialInterface();
private:
	uint8_t readSize;
    char  buffer[256];
    String * s;
    void printMessage();
    char * getMessageBuf();
	uint8_t * getMessageSize();

};

#endif /* SRC_SERIALINTERFACE_H_ */
