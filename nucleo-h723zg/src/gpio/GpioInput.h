/**
 * @file  GpioInput.h
 * @brief Single GPIO input pin backed by the STM32 HAL.
 *
 * Configures the pin as a floating or pulled digital input.
 * Intended for use with encoder channels, limit switches, buttons, etc.
 *
 * Usage example:
 * @code
 *   static GpioInput encA(GPIOA, GPIO_PIN_0);          // floating
 *   static GpioInput encB(GPIOA, GPIO_PIN_1, GPIO_PULLUP);
 *
 *   if (encA.read() == GPIO_PIN_SET) { ... }
 *   if (encA.isHigh()) { ... }
 * @endcode
 */
#pragma once

#include "GpioBase.h"

class GpioInput : public GpioBase {
public:
    /**
     * @param port  GPIO port  (e.g. GPIOA)
     * @param pin   HAL pin mask (e.g. GPIO_PIN_0)
     * @param pull  GPIO_NOPULL (default), GPIO_PULLUP, or GPIO_PULLDOWN
     */
    /**
     * @param mode  GPIO_MODE_INPUT (default, polling) or
     *              GPIO_MODE_IT_RISING_FALLING / GPIO_MODE_IT_RISING /
     *              GPIO_MODE_IT_FALLING for EXTI-interrupt use.
     *              When using an interrupt mode, also enable the relevant
     *              NVIC line (HAL_NVIC_EnableIRQ) in the caller.
     */
    GpioInput(GPIO_TypeDef* port, uint16_t pin,
              uint32_t pull = GPIO_NOPULL,
              uint32_t mode = GPIO_MODE_INPUT);

    ~GpioInput();

    /** @return GPIO_PIN_SET (HIGH) or GPIO_PIN_RESET (LOW). */
    [[nodiscard]] GPIO_PinState read() const;

    /** @return true if pin is HIGH. */
    [[nodiscard]] bool isHigh() const;

    /** @return true if pin is LOW. */
    [[nodiscard]] bool isLow() const;
};

