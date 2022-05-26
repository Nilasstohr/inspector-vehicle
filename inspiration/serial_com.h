//#include <stdio.h>
//#include <math.h>
//#include <string.h>
#include "WProgram.h"

#ifndef SERIAL_COM_H
#define SERIAL_COM_H

// Initialization
void serialInit(int baud);
void usbInit();

// USB
// int  usbWrite(const char *format, ...);
// void usbClear();
// //char *usbRead();
// 
// // Serial
// int serialWrite(const char *format, ...);
// int serialDataToQueue(const char *format, ...);
// bool serialWriteQueue();

#endif // SERIAL_COM_H