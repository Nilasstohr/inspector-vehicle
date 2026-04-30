/**
 * @file  GpioBase.h
 * @brief Shared port/pin storage and RCC clock-enable for GPIO classes.
 *
 * Not intended for use directly — inherit via GpioOutput or GpioInput.
 * No virtual functions: zero vtable overhead.
 */
#pragma once

#include "stm32h7xx_hal.h"

class GpioBase {
public:
    /* Non-copyable, non-movable — owns a hardware resource */
    GpioBase(const GpioBase&)            = delete;
    GpioBase& operator=(const GpioBase&) = delete;
    GpioBase(GpioBase&&)                 = delete;
    GpioBase& operator=(GpioBase&&)      = delete;

protected:
    GpioBase(GPIO_TypeDef* port, uint16_t pin, uint32_t mode, uint32_t pull);

    /** Enable the RCC clock for the given port (idempotent). */
    static void enablePortClock(GPIO_TypeDef* port);

    GPIO_TypeDef* const m_port;
    const uint16_t      m_pin;
};
