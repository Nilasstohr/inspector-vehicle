//
// Created by robotcentral on 4/22/26.
//

#ifndef UARTTRANSMITTER_H
#define UARTTRANSMITTER_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "stm32h7xx_hal.h"
#include <stm32h7xx_hal_uart.h>
#include "Transceiver.h"

class UartTransceiver final : public Transceiver{
public:
    explicit UartTransceiver(UART_HandleTypeDef & huart);
    tl::expected<void, TransceiverError> transmit(const char* data, uint16_t len) override;
    tl::expected<uint8_t, TransceiverError> receive(char* buf, uint16_t maxLen) override;
    [[nodiscard]] bool isDataAvailable() const override;

private:
    UART_HandleTypeDef  & m_huart;
    SemaphoreHandle_t   m_mutex{};
};


#endif //UARTTRANSMITTER_H
