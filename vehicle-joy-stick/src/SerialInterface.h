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
	enum SerialType {usb,serial1,serial2,serial5};

	SerialInterface();
	SerialInterface(SerialType type,uint32_t baudrate);
	bool hasMessage();
	bool hasMessage(SerialType type);
	String* getMessage();
	void printMessage(SerialType type);
	void printMessage();
	void sendMessage(String &message,SerialType type);
	bool validateCommand(uint8_t size, char id);
	void sendAck();
	virtual ~SerialInterface();
private:
	uint8_t readSize;
    char  buffer[256];
    String * s;

    char * getMessageBuf();
	uint8_t getMessageSize();
	void init(SerialType type);
	SerialType serialType;
	SerialType serialTypeTemp;
	// wrapper functions
	HardwareSerial * getHardwareSerial();
	int available();
	char read();
	void print(char c);
	void println();
	void changeSerialType(SerialType type);
	void restoreSerialType();

};

#endif /* SRC_SERIALINTERFACE_H_ */
