/*
 * SerialInterface.cpp
 *
 *  Created on: Jun 18, 2023
 *      Author: robot1
 */

#include "SerialInterface.h"

SerialInterface::SerialInterface(){
	init(SerialType::usb);
}

SerialInterface::SerialInterface(SerialType type,uint32_t baudrate){
	init(type);
	getHardwareSerial()->begin(baudrate);
}


void SerialInterface::init(SerialType type){
	readSize = 0;
	serialType = type;
	serialTypeTemp = type;
}

bool SerialInterface::hasMessage() {
	readSize = 0;
	if(available()){
		while (available()){
			char c = read();
			if(c=='\n'){
				//printMessage();
				return true;
			}
			buffer[readSize]=c;
			readSize++;
		}
	}
	readSize = 0;
	return false;
}


bool SerialInterface::hasMessage(SerialType type) {
	changeSerialType(type);
	hasMessage();
	restoreSerialType();
}


void SerialInterface::printMessage() {
	//Serial.print("received command: ");
	for(int i=0; i<readSize; i++){
		print(buffer[i]);
	}
	println();
}

void SerialInterface::sendMessage(String &message,SerialType type) {
	changeSerialType(type);
	for(int i=0; i< message.length(); i++){
		print(message[i]);
	}
	restoreSerialType();
}

void SerialInterface::printMessage(SerialType type)
{
	changeSerialType(type);
	printMessage();
	restoreSerialType();
}

String* SerialInterface::getMessage() {
	String * s = new String();
	for(int i=0; i<readSize; i++){
		s->append(buffer[i]);
	}
	return s;
}

char * SerialInterface::getMessageBuf() {
	return buffer;
}

uint8_t  SerialInterface::getMessageSize() {
	return readSize;
}

void SerialInterface::sendAck() {
	Serial.println("ack");
}
SerialInterface::~SerialInterface() {

}

bool SerialInterface::validateCommand(uint8_t size, char id) {
	return getMessageSize()==size && getMessageBuf()[0]==id;
}

int SerialInterface::available() {
	if(serialType==SerialType::usb){
		return Serial.available();
	}else{
		return getHardwareSerial()->available();
	}

}

char SerialInterface::read() {
	if(serialType==SerialType::usb){
		return Serial.read();
	}else{
		return getHardwareSerial()->read();
	}
}

void SerialInterface::print(char c) {
	if(serialType==SerialType::usb){
		Serial.print(c);
	}else{
		getHardwareSerial()->print(c);
	}
}

void SerialInterface::println() {
	if(serialType==SerialType::usb){
		Serial.println();
	}else{
		getHardwareSerial()->println();
	}
}

HardwareSerial * SerialInterface::getHardwareSerial(){
	switch(serialType){
		case SerialType::serial1:{
				return &Serial1;
			}
		case SerialType::serial2:{
			return &Serial2;
		}
	}
	return NULL;
}

void SerialInterface::changeSerialType(SerialType type) {
	serialTypeTemp = serialType;
	serialType = type;
}

void SerialInterface::restoreSerialType() {
	serialType=serialTypeTemp;
}

