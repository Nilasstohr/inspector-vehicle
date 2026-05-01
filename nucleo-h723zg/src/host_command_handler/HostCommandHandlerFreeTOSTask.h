//
// Created by robotcentral on 4/23/26.
//

#pragma once

#include <freetos/FreeTOSTask.h>
#include <motors/Encoder.h>
#include <uart/Uart.h>
#include <uart/UartTransceiver.h>

#ifndef HOSTCOMMANDHANDLERFREETOSTASK_H
#define HOSTCOMMANDHANDLERFREETOSTASK_H


class HostCommandHandlerFreeTOSTask final : public FreeTOSTask {
public:
    HostCommandHandlerFreeTOSTask(const Uart &uart);
private:
    void run() override;
    const Uart  &m_uart;
};


#endif //HOSTCOMMANDHANDLERFREETOSTASK_H
