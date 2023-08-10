//
// Created by robot1 on 8/5/23.
//

#include "UsbSerial.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h> // needed for memset
void UsbSerial::begin() {

    // https://en.wikibooks.org/wiki/Serial_Programming/termios

    m_serial_id = open("/dev/ttyACM0", O_RDWR);
    fcntl(m_serial_id, F_SETFL, FNDELAY);

    struct termios tio;
    struct termios stdio;

    memset(&stdio,0,sizeof(stdio));
    stdio.c_iflag=0;
    stdio.c_oflag=0;
    stdio.c_cflag=0;
    stdio.c_lflag=0;
    stdio.c_cc[VMIN]=1;
    stdio.c_cc[VTIME]=0;
    tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
    tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;

    m_serial_id=open("/dev/ttyACM0", O_RDWR | O_NONBLOCK);        // O_NONBLOCK might override VMIN and VTIME, so read() may return immediately.
    cfsetospeed(&tio,B4000000);            // 115200 baud
    cfsetispeed(&tio,B4000000);            // 115200 baud

    tcsetattr(m_serial_id,TCSANOW,&tio);
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
    write(m_serial_id, string->c_str(), string->size());
}


void UsbSerial::end() {
    close(m_serial_id);
}

void UsbSerial::flush() {
    tcflush(m_serial_id,TCIOFLUSH);
}
