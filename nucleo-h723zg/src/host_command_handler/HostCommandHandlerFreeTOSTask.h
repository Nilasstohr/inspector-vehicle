//
// Created by robotcentral on 4/23/26.
//

#pragma once

#include <freetos/FreeTOSTask.h>
#include <motors/Encoder.h>
#include <motors/PiMotorControl.h>
#include <uart/Uart.h>
#include <uart/UartTransceiver.h>

#ifndef HOSTCOMMANDHANDLERFREETOSTASK_H
#define HOSTCOMMANDHANDLERFREETOSTASK_H


class HostCommandHandlerFreeTOSTask final : public FreeTOSTask{
public:
    HostCommandHandlerFreeTOSTask(const Uart & uart, PiMotorControl & pi_motor_control);

private:
    void run() override;
    const Uart  &m_uart;
    PiMotorControl &m_pi_motor_control;;
};


#endif //HOSTCOMMANDHANDLERFREETOSTASK_H
