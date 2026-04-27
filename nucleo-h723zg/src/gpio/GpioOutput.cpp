/**
 * @file  GpioOutput.cpp
 * @brief HAL-based single push-pull GPIO output driver.
 */

#include "GpioOutput.h"

/* ── Helper: map a GPIO_TypeDef* to its RCC enable macro ─────────────────── */
static void enableGpioPortClock(GPIO_TypeDef* port) {
    if      (port == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
    else if (port == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
    else if (port == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
    else if (port == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
    else if (port == GPIOE) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
    else if (port == GPIOF) { __HAL_RCC_GPIOF_CLK_ENABLE(); }
    else if (port == GPIOG) { __HAL_RCC_GPIOG_CLK_ENABLE(); }
    else if (port == GPIOH) { __HAL_RCC_GPIOH_CLK_ENABLE(); }
    else if (port == GPIOJ) { __HAL_RCC_GPIOJ_CLK_ENABLE(); }
    else if (port == GPIOK) { __HAL_RCC_GPIOK_CLK_ENABLE(); }
}

/* ── constructor ──────────────────────────────────────────────────────────── */
GpioOutput::GpioOutput(GPIO_TypeDef* port,
                       uint16_t      pin,
                       GPIO_PinState initState)
    : m_port(port), m_pin(pin)
{
    enableGpioPortClock(port);

    GPIO_InitTypeDef init = {};
    init.Pin   = pin;
    init.Mode  = GPIO_MODE_OUTPUT_PP;
    init.Pull  = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(port, &init);

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
void GpioOutput::write(GPIO_PinState state) const {
    HAL_GPIO_WritePin(m_port, m_pin, state);
}

/* ── read ─────────────────────────────────────────────────────────────────── */
GPIO_PinState GpioOutput::read() const {
    return HAL_GPIO_ReadPin(m_port, m_pin);
}

