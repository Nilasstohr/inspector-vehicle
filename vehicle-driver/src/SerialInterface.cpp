/*
 * SerialInterface.cpp
 *
 *  Created on: Jun 18, 2023
 *      Author: robot1
 */

#include "SerialInterface.h"

SerialInterface::SerialInterface(){
	readSize = 0;
}

bool SerialInterface::availableRead() {
	readSize = 0;
	if(Serial.available()){
		while (Serial.available()){
			char c = Serial.read();
			if(c=='\n'){
				printReceived();
				return true;
			}
			buffer[readSize]=c;
			readSize++;
		}
	}
	readSize = 0;
	return false;
}

void SerialInterface::printReceived() {
	Serial.println("received command: ");
	for(int i=0; i<readSize; i++){
		Serial.print(buffer[i]);
	}
	Serial.println();

}
String* SerialInterface::getReceived() {
	String *s = new String();
	for(int i=0; i<readSize; i++){
		s->append(buffer[i]);
	}
	return s;
}


SerialInterface::~SerialInterface() {

}

