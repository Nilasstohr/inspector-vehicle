/**
 * @file  GpioInput.cpp
 * @brief HAL-based single GPIO input driver.
 */

#include "GpioInput.h"

GpioInput::GpioInput(GPIO_TypeDef* const port, const uint16_t pin,
                     const uint32_t pull, const uint32_t mode)
    : GpioBase(port, pin, mode, pull) {}

GpioInput::~GpioInput() {
    HAL_GPIO_DeInit(m_port, m_pin);
}

GPIO_PinState GpioInput::read() const {
    return HAL_GPIO_ReadPin(m_port, m_pin);
}

bool GpioInput::isHigh() const {
    return read() == GPIO_PIN_SET;
}

bool GpioInput::isLow() const {
    return read() == GPIO_PIN_RESET;
}
