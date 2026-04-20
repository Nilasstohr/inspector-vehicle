//
// Created by robotcentral on 4/23/26.
//

#pragma once

#include <uart/Uart.h>
#include <uart/UartTransceiver.h>

#include "FreeRTOS.h"
#include "stm32h7xx_hal.h"
#include "task.h"


#ifndef HOSTCOMMANDHANDLERFREETOSTASK_H
#define HOSTCOMMANDHANDLERFREETOSTASK_H


class HostCommandHandlerFreeTOSTask {
public:
    HostCommandHandlerFreeTOSTask(UartTransceiver &transceiver, uint32_t intervalMs);
    HostCommandHandlerFreeTOSTask(const HostCommandHandlerFreeTOSTask&)            = delete;
    HostCommandHandlerFreeTOSTask& operator=(const HostCommandHandlerFreeTOSTask&) = delete;
    HostCommandHandlerFreeTOSTask(HostCommandHandlerFreeTOSTask&&)                 = delete;
    HostCommandHandlerFreeTOSTask& operator=(HostCommandHandlerFreeTOSTask&&)      = delete;

    Uart& getUart() { return m_uart; }
    void start(UBaseType_t priority = 1, uint16_t stackWords = 512);

private:
    static void taskEntry(void* arg);
    void run() const;
    TaskHandle_t m_handle = nullptr;
    uint32_t m_intervalMs;
    Uart m_uart;
};



#endif //HOSTCOMMANDHANDLERFREETOSTASK_H
