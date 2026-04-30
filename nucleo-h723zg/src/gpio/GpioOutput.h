#pragma once

#include "GpioBase.h"

/**
 * @class GpioOutput
 * @brief Single push-pull GPIO output pin backed by the STM32 HAL.
 *
 * Usage example (D7 — PG12):
 * @code
 *   static GpioOutput d7(GPIOG, GPIO_PIN_12);
 *   d7.setHigh();
 *   d7.toggle();
 *   d7.setLow();
 * @endcode
 */
class GpioOutput : public GpioBase {
public:
    /**
     * @param port       GPIO port (e.g. GPIOG)
     * @param pin        HAL pin mask (e.g. GPIO_PIN_12)
     * @param initState  Initial logic level; defaults to GPIO_PIN_RESET (LOW)
     */
    GpioOutput(GPIO_TypeDef* port, uint16_t pin,
               GPIO_PinState initState = GPIO_PIN_RESET);

    ~GpioOutput();

    /** Drive the pin HIGH. */
    void setHigh() const;

    /** Drive the pin LOW. */
    void setLow() const;

    /** Toggle the pin state. */
    void toggle() const;

    /** Write an explicit logic level. */
    void write(GPIO_PinState state) const;

    /** @return Current output state. */
    [[nodiscard]] GPIO_PinState read() const;
};
