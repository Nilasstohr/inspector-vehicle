/**
 * @file  GpioOutput.cpp
 * @brief HAL-based single push-pull GPIO output driver.
 */

#include "GpioOutput.h"

/* ── constructor ──────────────────────────────────────────────────────────── */
GpioOutput::GpioOutput(GPIO_TypeDef* const port, const uint16_t pin,
                       const GPIO_PinState initState)
    : GpioBase(port, pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL)
{
    HAL_GPIO_WritePin(port, pin, initState);
}

/* ── destructor ───────────────────────────────────────────────────────────── */
GpioOutput::~GpioOutput() {
    HAL_GPIO_WritePin(m_port, m_pin, GPIO_PIN_RESET);
    HAL_GPIO_DeInit(m_port, m_pin);
}

/* ── setHigh ──────────────────────────────────────────────────────────────── */
void GpioOutput::setHigh() const {
    HAL_GPIO_WritePin(m_port, m_pin, GPIO_PIN_SET);
}

/* ── setLow ───────────────────────────────────────────────────────────────── */
void GpioOutput::setLow() const {
    HAL_GPIO_WritePin(m_port, m_pin, GPIO_PIN_RESET);
}

/* ── toggle ───────────────────────────────────────────────────────────────── */
void GpioOutput::toggle() const {
    HAL_GPIO_TogglePin(m_port, m_pin);
}

/* ── write ────────────────────────────────────────────────────────────────── */
void GpioOutput::write(const GPIO_PinState state) const {
    HAL_GPIO_WritePin(m_port, m_pin, state);
}

/* ── read ─────────────────────────────────────────────────────────────────── */
GPIO_PinState GpioOutput::read() const {
    return HAL_GPIO_ReadPin(m_port, m_pin);
}
