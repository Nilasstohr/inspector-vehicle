//
// Created by robot1 on 8/23/24.
//

#include "SerialInterfaceStub.h"

SerialInterfaceStub::SerialInterfaceStub(const char *serialDevice):
SerialInterface(serialDevice) {}

void SerialInterfaceStub::sendRequest(char *text) {}
void SerialInterfaceStub::sendRequest(std::string *text) {}
