//
// Created by robotcentral on 4/30/26.
//

#include "ControllerFreeTOSTask.h"

#include <uart/Uart.h>
#include <uart/UartTransceiver.h>

ControllerFreeTOSTask::ControllerFreeTOSTask(const Uart & uart,const Encoder & encoder1,const Encoder & encoder2) :
m_uart(uart),m_encoder1(encoder1),m_encoder2(encoder2) {}


void ControllerFreeTOSTask::run() const {
    while (true) {
        m_uart.logf("ControllerFreeTosTask Encoders : m1: %ld m2: %ld\r\n", m_encoder1.getCount(),m_encoder2.getCount());
        vTaskDelay(pdMS_TO_TICKS(50)); // Adjust the delay as needed
    }
}
