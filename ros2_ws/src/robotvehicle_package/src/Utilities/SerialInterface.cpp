//
// Created by robot1 on 8/4/23.
//

#include <chrono>
#include <rclcpp/utilities.hpp>
#include <stdexcept>
#include <thread>
#include "SerialInterface.h"

#include <iostream>

#define ACK "ack"

// https://www.linkedin.com/pulse/linux-serial-port-programming-c-mohammad-t-abdoli

SerialInterface::SerialInterface(const char *serialDevice) {
    //Serial =  UsbSerial();
    //Serial.setSerialDevice(serialDevice);
   // m_buffer = new std::string();
    //Serial.begin();
    // STM32 CDC VCP needs ~500 ms after the host opens the port before it
    // is ready to receive — skip this and the first command gets lost.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    awaitTimer = new AwaitTimer(5000);
}

void SerialInterface::reopen() {
    Serial.end();
    Serial.begin();
}

bool SerialInterface::hasResponse() {
    std::cout << "\nchecking for response: " << *m_buffer << " ";
    // Read all currently available bytes without sleeping inside the loop.
    // If no '\n' is found this call, the outer polling loop in send() will
    // retry after a short sleep, allowing more bytes to arrive.
    while (Serial.available()) {
        const char c = Serial.readChar();
        std::cout << c;
        if (c == '\n') {
            std::cout << "response received: " << *m_buffer << std::endl;
            return true;
        }
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


void SerialInterface::sendRequest(const char *text) {
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
    std::cout << "Sending request: " << *m_buffer << std::endl;
    Serial.writeString(m_buffer);
    m_buffer->clear();  // done with the request; now accumulate the response
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
        throw std::runtime_error("invalid response: missing acknowledgment (got: " + *response + ")");
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

