/**
 * @file    HardwareTimer.cpp
 * @brief   TIM2 hardware setup — 100 µs update period.
 *
 * Clock maths (SYSCLK = 64 MHz, APB1 prescaler = 1):
 *   PSC = 63  →  Timer clock = 64 MHz / (63 + 1) = 1 MHz
 *   ARR = 99  →  Period      = (99 + 1) / 1 MHz   = 100 µs
 */

#include "timer/HardwareTimer.h"

HardwareTimer::HardwareTimer()
{
    m_htim.Instance               = TIM2;
    m_htim.Init.Prescaler         = kPrescaler;    /* 63  →  1 MHz timer clock  */
    m_htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
    m_htim.Init.Period            = kPeriod;       /* 99  →  100 µs period      */
    m_htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
}

HAL_StatusTypeDef HardwareTimer::start()
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    const HAL_StatusTypeDef status = HAL_TIM_Base_Init(&m_htim);
    if (status != HAL_OK) {
        return status;
    }

    HAL_NVIC_SetPriority(TIM2_IRQn, kIrqPriority, /*subpriority=*/0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    return HAL_TIM_Base_Start_IT(&m_htim);
}

void HardwareTimer::stop()
{
    HAL_TIM_Base_Stop_IT(&m_htim);
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
    HAL_TIM_Base_DeInit(&m_htim);
    __HAL_RCC_TIM2_CLK_DISABLE();
}
