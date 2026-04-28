#pragma once

#include "stm32h7xx_hal.h"

/**
 * @class PwmOutput
 * @brief Single-channel PWM output backed by a STM32 HAL TIM handle.
 *
 * Usage example (~20 kHz, ~11.6-bit on TIM1 CH1, PE9):
 * @code
 *   static PwmOutput leftMotor(TIM1, TIM_CHANNEL_1);  // defaults: PSC=0, ARR=3199
 *   leftMotor.start();
 *   leftMotor.setMotorPwm(1600);  // 50 %
 * @endcode
 *
 * Clock maths (SYSCLK = 64 MHz HSI, no PLL):
 *   Timer clock  = SYSCLK / (PSC + 1)
 *   PWM period   = (ARR + 1) ticks
 *   PWM freq     = Timer clock / (ARR + 1)
 *
 * Defaults (PSC=0, ARR=3199):
 *   Timer clock = 64 MHz / 1     = 64 MHz
 *   PWM freq    = 64 MHz / 3200  = 20 000 Hz
 *   Resolution  = log2(3200)     ≈ 11.64 bits  (3200 steps)
 */
class PwmOutput {
public:
    /**
     * @param instance   TIM peripheral (e.g. TIM3)
     * @param channel    HAL channel constant (TIM_CHANNEL_1 … TIM_CHANNEL_4)
     * @param prescaler  Timer prescaler register value (PSC), 0-based
     * @param period     Auto-reload register value (ARR), 0-based
     */
    PwmOutput(TIM_TypeDef* instance,
              uint32_t     channel,
              uint32_t     prescaler = 0,
              uint32_t     period    = 3199);

    PwmOutput(const PwmOutput&)            = delete;
    PwmOutput& operator=(const PwmOutput&) = delete;
    PwmOutput(PwmOutput&&)                 = delete;
    PwmOutput& operator=(PwmOutput&&)      = delete;

    /** Initialise the timer and start PWM output at 0 % duty cycle. */
    void start();

    /** Stop the PWM output and de-initialise the timer. */
    void stop();

    /**
     * Set PWM output directly by raw timer compare value.
     * @param pwm  0 … ARR (default ARR=3199 for ~11.6-bit, 3200 steps)
     *             0     = 0 % duty cycle (fully off)
     *             3199  = 100 % duty cycle (fully on)
     * Value is clamped to [0, ARR] automatically.
     */
    void setPwmRawValue(uint16_t pwm);

    /** @return Current raw PWM compare value (0 … ARR). */
    [[nodiscard]] uint16_t getMotorPwm() const { return m_pwm; }

    /** @return Underlying HAL TIM handle (for advanced use). */
    TIM_HandleTypeDef& handle() { return m_htim; }

private:
    TIM_HandleTypeDef m_htim  = {};
    uint32_t          m_channel;
    uint16_t          m_pwm   = 0;
};





