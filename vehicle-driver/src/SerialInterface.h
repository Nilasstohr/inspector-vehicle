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
	enum SerialType {usb,serial2,serial5};

	SerialInterface();
	SerialInterface(SerialType type,uint32_t baudrate);
	bool hasMessage();
	bool hasMessage(SerialType type);
	String* getMessage();
	void printMessage(SerialType type);
	void printMessage();
	void sendMessage(String &message,SerialType type);
	bool validateCommand(uint8_t size, char id);
	bool validateCommandWithArgs(uint8_t argsSize,char id);
	double getArg(int index);
	void sendAck();
	virtual ~SerialInterface();
private:
	uint8_t readSize;
    char  buffer[256];
    String * s;
    // args
    double args[2];
    int argsSize;
    char arg[20];
    double atofn(char *src, int n);
    char * getMessageBuf();
	uint8_t * getMessageSize();
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
