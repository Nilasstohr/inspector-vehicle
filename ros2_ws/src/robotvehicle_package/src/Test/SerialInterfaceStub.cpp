//
// Created by robot1 on 8/23/24.
//

#include "SerialInterfaceStub.h"

#include <iostream>
#include <ostream>

SerialInterfaceStub::SerialInterfaceStub(const char *serialDevice):
SerialInterface(serialDevice) {}

void SerialInterfaceStub::sendRequest(const char *text) {
    std::cout << "emulated sendRequest:" << text << std::endl;
}
void SerialInterfaceStub::sendRequest(std::string *text) {
    std::cout << "emulated sendRequest:" << text << std::endl;
}
