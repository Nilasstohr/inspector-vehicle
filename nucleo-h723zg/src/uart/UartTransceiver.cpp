//
// Created by robotcentral on 4/22/26.
//

#include "UartTransceiver.h"
#include <portmacro.h>

UartTransceiver::UartTransceiver(UART_HandleTypeDef & huart) : m_huart(huart) {
    m_mutex = xSemaphoreCreateMutex();
}

tl::expected<void, TransceiverError> UartTransceiver::transmit(const char *data, const uint16_t len) const {
    if (len == 0) return {};  // success — nothing to send

    /* Skip mutex before the scheduler starts — xSemaphoreTake would assert. */
    const bool schedulerRunning = (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);

    if (schedulerRunning) {
        if (xSemaphoreTake(m_mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
            return tl::unexpected(TransceiverError::Busy);
        }
    }

    const HAL_StatusTypeDef status =
        HAL_UART_Transmit(&m_huart, reinterpret_cast<const uint8_t*>(data), len, /*timeout_ms=*/100);

    if (schedulerRunning) {
        xSemaphoreGive(m_mutex);
    }

    if (status == HAL_TIMEOUT) return tl::unexpected(TransceiverError::Timeout);
    if (status != HAL_OK)      return tl::unexpected(TransceiverError::Busy);
    return {};  // success
}

tl::expected<uint8_t, TransceiverError> UartTransceiver::receive(char *buf, const uint16_t maxLen) {
    // Clear stale IDLE flag so ReceiveToIdle doesn't return immediately
    // on data that arrived while we were busy (e.g. transmitting an echo).
    __HAL_UART_CLEAR_FLAG(&m_huart, UART_CLEAR_IDLEF);

    uint16_t bytesRead = 0;
    const HAL_StatusTypeDef status =
        HAL_UARTEx_ReceiveToIdle(&m_huart, reinterpret_cast<uint8_t*>(buf), maxLen - 1, &bytesRead, 50);

    if (status == HAL_TIMEOUT) return tl::unexpected(TransceiverError::Timeout);
    if (status != HAL_OK)      return tl::unexpected(TransceiverError::Busy);
    return static_cast<uint8_t>(bytesRead);
}

bool UartTransceiver::isDataAvailable() const {
    return __HAL_UART_GET_FLAG(&m_huart, UART_FLAG_RXNE) == SET;
}

//VEL 3.3 3.3;
