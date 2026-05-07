//
// Created by robot1 on 8/5/23.
//

#include "UsbSerial.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>   // needed for memset
#include <stdexcept>
#include <string>
void UsbSerial::begin() {

    // https://en.wikibooks.org/wiki/Serial_Programming/termios

    // Single open: O_NOCTTY = don't become controlling terminal,
    // O_NONBLOCK = don't block waiting for carrier detect.
    m_serial_id = open(serialDevice, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (m_serial_id < 0) {
        throw std::runtime_error(std::string("Failed to open serial device: ") + serialDevice);
    }

    struct termios tio;
    memset(&tio, 0, sizeof(tio));
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_cflag = CS8 | CREAD | CLOCAL;    // 8n1
    tio.c_lflag = 0;
    tio.c_cc[VMIN]  = 1;
    tio.c_cc[VTIME] = 5;

    cfsetospeed(&tio, B115200);             // 115200 baud
    cfsetispeed(&tio, B115200);             // 115200 baud

    tcsetattr(m_serial_id, TCSANOW, &tio);
}

bool UsbSerial::available(){
    if (read(m_serial_id, &m_char_last_read, 1) > 0){
        return true;
    }else{
        return false;
    }
}

char UsbSerial::readChar() {
    return m_char_last_read;
}

void UsbSerial::writeString(std::string *string) {
    ssize_t size =  write(m_serial_id, string->c_str(), string->size());
    (void)size;
}


void UsbSerial::end() {
    close(m_serial_id);
}

void UsbSerial::flush() {
    tcflush(m_serial_id,TCIOFLUSH);
}

void UsbSerial::setSerialDevice(const char *serialDevice) {
    this->serialDevice = serialDevice;
}
