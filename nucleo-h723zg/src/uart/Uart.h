#pragma once


#include <stdarg.h>
#include <stddef.h>

#include "Transceiver.h"

/**
 * @class Uart
 * @brief Thread-safe UART debug logger.
 *
 * Depends on the Transmitter interface so it can be used with any
 * concrete transmitter (e.g. UartTransmitter on-target, or a mock in tests).
 *
 * Typical setup (Nucleo-H723ZG — USART3 exposed as ST-Link VCP):
 *   UART_HandleTypeDef huart3;
 *   UART3_Init(&huart3);              // configure HAL handle + GPIO
 *   UartTransmitter tx(&huart3);
 *   UartLogger logger(tx);
 *   logger.println("Boot OK");
 */
class Uart
{
public:
    explicit Uart(Transceiver & transceiver);

    Uart(const Uart&)            = delete;
    Uart& operator=(const Uart&) = delete;
    Uart(Uart&&)                 = delete;
    Uart& operator=(Uart&&)      = delete;

    /** Send a null-terminated string without a newline. */
    void print(const char* str) const;

    /** Send a null-terminated string followed by CR+LF. */
    void println(const char* str) const;

    /** printf-style formatted output (max 256 bytes per call). */
    void logf(const char* fmt, ...) const __attribute__((format(printf, 2, 3)));

    Transceiver & getTransceiver() const;
private:
    Transceiver & m_transceiver;
};
