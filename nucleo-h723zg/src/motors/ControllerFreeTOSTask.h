//
// Created by robotcentral on 4/30/26.
//

#ifndef CONTROLLERFREETOSTASK_H
#define CONTROLLERFREETOSTASK_H
#include <FreeTOSTask.h>
#include <uart/Uart.h>
#include <uart/UartTransceiver.h>

#include "Encoder.h"


class ControllerFreeTOSTask final : public FreeTOSTask{
public:
    ControllerFreeTOSTask(const Uart &uart, const Encoder &encoder1,const Encoder &encoder2);
    void run() const override;
    const Uart &m_uart;
    const Encoder &m_encoder1;
    const Encoder &m_encoder2;
};



#endif //CONTROLLERFREETOSTASK_H
