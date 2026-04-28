/**
 * @file  GpioBase.cpp
 * @brief Shared RCC clock-enable and constructor for GpioOutput / GpioInput.
 */

#include "GpioBase.h"

GpioBase::GpioBase(GPIO_TypeDef* const port, const uint16_t pin,
                   const uint32_t mode, const uint32_t pull)
    : m_port(port), m_pin(pin)
{
    enablePortClock(port);

    GPIO_InitTypeDef init = {};
    init.Pin   = pin;
    init.Mode  = mode;
    init.Pull  = pull;
    init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(port, &init);
}

void GpioBase::enablePortClock(GPIO_TypeDef* const port) {
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
