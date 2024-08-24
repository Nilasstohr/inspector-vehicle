//
// Created by robot1 on 8/4/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_SERIALINTERFACE_H
#define ROBOTVEHICLE_PACKAGE_SERIALINTERFACE_H


#include "UsbSerial.h"
#include <string>
#include "AwaitTimer.h"

class SerialInterface {
public:
    SerialInterface(const char *serialDevice);
    bool hasResponse();
    std::string * getResponse();
    virtual void sendRequest(char *text) ;
    virtual void sendRequest(std::string *text);
    virtual ~SerialInterface();
    void close();

    void reopen();

    void flush();

private:
    UsbSerial Serial;
    std::string *m_buffer;
    AwaitTimer *awaitTimer;
    void validateResponse(std::string *response);
    void stripAck(std::string *str);
    void send();
};


#endif //ROBOTVEHICLE_PACKAGE_SERIALINTERFACE_H
