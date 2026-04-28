/**
 * @file  PwmOutput.cpp
 * @brief HAL-based single-channel PWM output driver.
 */

#include "PwmOutput.h"

/* ── constructor ──────────────────────────────────────────────────────────── */
PwmOutput::PwmOutput(TIM_TypeDef* instance,
                     uint32_t     channel,
                     uint32_t     prescaler,
                     uint32_t     period)
    : m_channel(channel)
{
    m_htim.Instance               = instance;
    m_htim.Init.Prescaler         = prescaler;
    m_htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
    m_htim.Init.Period            = period;
    m_htim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    m_htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
}

/* ── start ────────────────────────────────────────────────────────────────── */
void PwmOutput::start() {
    /* Initialise the base timer */
    HAL_TIM_PWM_Init(&m_htim);

    /* Configure the PWM channel */
    TIM_OC_InitTypeDef oc = {};
    oc.OCMode       = TIM_OCMODE_PWM1;
    oc.Pulse        = 0;                    /* 0 % duty cycle on start */
    oc.OCPolarity   = TIM_OCPOLARITY_HIGH;
    oc.OCFastMode   = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&m_htim, &oc, m_channel);

    /* Start PWM generation */
    HAL_TIM_PWM_Start(&m_htim, m_channel);
}

/* ── stop ─────────────────────────────────────────────────────────────────── */
void PwmOutput::stop() {
    HAL_TIM_PWM_Stop(&m_htim, m_channel);
    HAL_TIM_PWM_DeInit(&m_htim);
}

/* ── setMotorPwm ──────────────────────────────────────────────────────────── */
void PwmOutput::setPwmRawValue(const uint16_t pwm) {
    /* Step 1 — Read the Auto-Reload Register (ARR) value.
     *   ARR is the timer's period ceiling, set in the constructor.
     *   Default: 8191 (13-bit resolution → 8192 discrete steps, 0…8191).
     *   The timer counter counts from 0 up to ARR, then resets.
     *   A CCR value above ARR would mean "always on" / undefined — so we clamp. */
    uint32_t arr = m_htim.Init.Period;

    /* Step 2 — Clamp the requested value to the valid range [0, ARR].
     *   If pwm > ARR (e.g. caller passes 9000 with ARR=8191), cap it to ARR.
     *   This prevents writing an invalid compare value to the hardware register. */
    m_pwm = (pwm > arr) ? static_cast<uint16_t>(arr) : pwm;

    /* Step 3 — Write the clamped value into the Capture/Compare Register (CCR).
     *   The timer hardware compares its running counter against CCR every tick:
     *     counter < CCR  → output pin is HIGH
     *     counter >= CCR → output pin is LOW
     *   So:  CCR = 0    → always LOW  (0 % duty cycle, motor off)
     *        CCR = 4096 → HIGH for half the period (~50 %)
     *        CCR = 8191 → HIGH for almost the full period (~100 %)
     *   __HAL_TIM_SET_COMPARE is a macro that writes directly to the CCRx
     *   register without stopping the timer — the change takes effect on the
     *   next timer cycle with no glitch. */
    __HAL_TIM_SET_COMPARE(&m_htim, m_channel, m_pwm);
}




