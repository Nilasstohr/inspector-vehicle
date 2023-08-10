//
// Created by robot1 on 8/5/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_USBSERIAL_H
#define ROBOTVEHICLE_PACKAGE_USBSERIAL_H

#include <string>

class UsbSerial {
public:
    void begin();
    bool available();
    char readChar();
    void writeString(std::string *string);
    void end();

    void flush();

private:
    int m_serial_id;
    char m_char_last_read;
    char read_buf [256];
};


#endif //ROBOTVEHICLE_PACKAGE_USBSERIAL_H
