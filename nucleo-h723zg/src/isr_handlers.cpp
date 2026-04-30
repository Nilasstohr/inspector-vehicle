/**
 * @file    isr_handlers.cpp
 * @brief   All extern "C" ISR definitions for the project.
 *
 * Pin names come from MotorPinConfig.h — change a pin there and this
 * file picks it up automatically.  No encoder logic lives here; each
 * handler clears the EXTI pending flag then trampolines into Encoder.
 *
 * IMPORTANT: the EXTI pending flag MUST be cleared before returning.
 * Omitting the clear causes the CPU to re-enter the ISR immediately on
 * every cycle, starving FreeRTOS and making the encoder appear dead.
 *
 * Encoder pins must be on EXTI lines 0-4 to get dedicated vectors.
 * Lines 5-9 share EXTI9_5_IRQHandler; lines 10-15 share EXTI15_10_IRQHandler.
 */

#include "motors/Encoder.h"
#include "motors/MotorPinConfig.h"

extern "C" {

/* ── Motor 1 encoder ───────────────────────────────────────────────────── */
void EXTI3_IRQHandler(void) {   /* PB1 — MOTOR1_ENC_A_PIN */
    __HAL_GPIO_EXTI_CLEAR_IT(MOTOR1_ENC_A_PIN);
    Encoder::isrChannelA(0);
}

void EXTI4_IRQHandler(void) {   /* PB2 — MOTOR1_ENC_B_PIN */
    __HAL_GPIO_EXTI_CLEAR_IT(MOTOR1_ENC_B_PIN);
    Encoder::isrChannelB(0);
}

/* ── Motor 2 ncoder ───────────────────────────────────────────────────── */
void EXTI1_IRQHandler(void) {   /* PB1 — MOTOR1_ENC_A_PIN */
    __HAL_GPIO_EXTI_CLEAR_IT(MOTOR2_ENC_A_PIN);
    Encoder::isrChannelA(1);
}

void EXTI2_IRQHandler(void) {   /* PB2 — MOTOR1_ENC_B_PIN */
    __HAL_GPIO_EXTI_CLEAR_IT(MOTOR2_ENC_B_PIN);
    Encoder::isrChannelB(1);
}




} // extern "C"
