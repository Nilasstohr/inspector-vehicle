//
// Created by robot1 on 8/4/23.
//

#include <chrono>
#include <rclcpp/utilities.hpp>
#include <thread>
#include "SerialInterface.h"

#define ACK "ack"

// https://www.linkedin.com/pulse/linux-serial-port-programming-c-mohammad-t-abdoli

SerialInterface::SerialInterface(const char *serialDevice) {
    Serial =  UsbSerial();
    Serial.setSerialDevice(serialDevice);
    m_buffer = new std::string();
    Serial.begin();
    awaitTimer = new AwaitTimer(5000);
}

void SerialInterface::reopen() {
    Serial.end();
    Serial.begin();
}

bool SerialInterface::hasResponse() {
    m_buffer->clear();
    char c;
    while (Serial.available()){
        c = Serial.readChar();
        if(c=='\n')
            return true;
        m_buffer->push_back(c);
    }
    return false;
}

std::string * SerialInterface::getResponse() {
    return m_buffer;
}

void SerialInterface::stripAck(std::string *str) {
    size_t start_pos = str->find(ACK);
    if(start_pos == std::string::npos)
        return;
    str->replace(start_pos, sizeof ACK, "");
}


void SerialInterface::sendRequest(char *text) {
    m_buffer->clear();
    m_buffer->append(text);
    send();
 }

void SerialInterface::sendRequest(std::string *text) {
    m_buffer->clear();
    m_buffer->append(text->c_str());
    send();
}

void SerialInterface::send(){
    m_buffer->push_back('\n');
    Serial.writeString(m_buffer);
    awaitTimer->start();
    while(!awaitTimer->hasWaitingTimeExceeded()){
        if(hasResponse()){
            validateResponse(getResponse());
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    throw std::runtime_error("no response from device within accepted time");
}

void SerialInterface::validateResponse(std::string *response) {
    if (response->find(ACK) == std::string::npos) {
        throw "invalid response missing acknowledgment";
    }
    stripAck(response);
}

SerialInterface::~SerialInterface() {

}

void SerialInterface::close() {
    Serial.end();
}

void SerialInterface::flush() {
    Serial.flush();
}

