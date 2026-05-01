/**
 * @file    HardwareTimer.h
 * @brief   TIM2 periodic timer — hardware setup only.
 *
 * Configures TIM2 for a 100 µs update period and enables its interrupt.
 * The caller is responsible for providing a TIM2_IRQHandler that clears
 * the update flag and dispatches to application logic (see isr_handlers.cpp).
 *
 * Clock maths (SYSCLK = 64 MHz HSI, APB1 prescaler = 1):
 *   PSC = 63  →  Timer clock = 64 MHz / 64 = 1 MHz
 *   ARR = 99  →  Period      = 100 / 1 MHz  = 100 µs  (10 kHz)
 */

#pragma once

#include "stm32h7xx_hal.h"

class HardwareTimer {
public:
    HardwareTimer();

    HardwareTimer(const HardwareTimer&)            = delete;
    HardwareTimer& operator=(const HardwareTimer&) = delete;
    HardwareTimer(HardwareTimer&&)                 = delete;
    HardwareTimer& operator=(HardwareTimer&&)      = delete;

    /** Start the timer and enable the TIM2 update-event interrupt. */
    [[nodiscard]] HAL_StatusTypeDef start();

    /** Stop the timer and disable the TIM2 interrupt. */
    void stop();

private:
    TIM_HandleTypeDef m_htim = {};

    /** PSC = 63  →  Timer clock = 64 MHz / 64 = 1 MHz                      */
    static constexpr uint32_t kPrescaler   = 63U;

    /** ARR = 99  →  Period = 100 ticks @ 1 MHz = 100 µs                    */
    static constexpr uint32_t kPeriod      = 99U;

    /** NVIC priority — must be ≥ configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY (5) */
    static constexpr uint32_t kIrqPriority = 6U;
};
