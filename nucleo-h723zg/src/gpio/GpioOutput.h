#pragma once

#include "stm32h7xx_hal.h"

/**
 * @class GpioOutput
 * @brief Single push-pull GPIO output pin backed by the STM32 HAL.
 *
 * Owns the clock-enable, init, and de-init lifecycle for one output pin.
 * The pin is driven LOW on construction and can be set, cleared, or toggled
 * at any time after construction.
 *
 * Usage example (D7 — PG12):
 * @code
 *   static GpioOutput d7(GPIOG, GPIO_PIN_12);
 *   d7.setHigh();
 *   d7.toggle();
 *   d7.setLow();
 * @endcode
 */
class GpioOutput {
public:
    /**
     * @param port       GPIO port (e.g. GPIOG)
     * @param pin        HAL pin mask (e.g. GPIO_PIN_12)
     * @param initState  Initial logic level; defaults to GPIO_PIN_RESET (LOW)
     */
    GpioOutput(GPIO_TypeDef*   port,
               uint16_t        pin,
               GPIO_PinState   initState = GPIO_PIN_RESET);

    ~GpioOutput();

    /* Non-copyable, non-movable — owns a hardware resource */
    GpioOutput(const GpioOutput&)            = delete;
    GpioOutput& operator=(const GpioOutput&) = delete;
    GpioOutput(GpioOutput&&)                 = delete;
    GpioOutput& operator=(GpioOutput&&)      = delete;

    /** Drive the pin HIGH. */
    void setHigh() const;

    /** Drive the pin LOW. */
    void setLow() const;

    /** Toggle the pin state. */
    void toggle() const;

    /**
     * Write an explicit logic level.
     * @param state  GPIO_PIN_SET (HIGH) or GPIO_PIN_RESET (LOW)
     */
    void write(GPIO_PinState state) const;

    /** @return Current output state (GPIO_PIN_SET or GPIO_PIN_RESET). */
    [[nodiscard]] GPIO_PinState read() const;

private:
    GPIO_TypeDef* m_port;
    uint16_t      m_pin;
};

