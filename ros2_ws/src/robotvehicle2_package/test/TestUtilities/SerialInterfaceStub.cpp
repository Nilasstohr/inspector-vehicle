//
// Created by robot1 on 8/23/24.
//

#include "SerialInterfaceStub.h"

#include <iostream>
#include <ostream>

SerialInterfaceStub::SerialInterfaceStub(const char *serialDevice):
SerialInterface(serialDevice) {}

void SerialInterfaceStub::sendRequest(const char *text) {
    (void)text;
}
void SerialInterfaceStub::sendRequest(std::string *text) {
    (void)text;
}
