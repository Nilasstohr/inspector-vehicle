//
// Created by robotcentral on 4/23/26.
//

#pragma once

#include <freetos/FreeTOSTask.h>
#include <motors/Encoder.h>
#include <motors/PiMotorControl.h>
#include <uart/Uart.h>
#include <uart/UartLineReceiver.h>
#include <uart/UartTransceiver.h>

#include "ReceiveCommand.h"

#ifndef HOSTCOMMANDHANDLERFREETOSTASK_H
#define HOSTCOMMANDHANDLERFREETOSTASK_H


class HostCommandHandlerFreeTOSTask final : public FreeTOSTask{
public:
    HostCommandHandlerFreeTOSTask(const Uart & uart, UartLineReceiver & line_receiver, PiMotorControl & pi_motor_control);

private:
    void run() override;

    void handleCommand(const ReceiveCommand &cmd, const std::string_view &received) const;

    const Uart  &m_uart;
    UartLineReceiver &m_line_receiver;
    PiMotorControl &m_pi_motor_control;;
};


#endif //HOSTCOMMANDHANDLERFREETOSTASK_H
