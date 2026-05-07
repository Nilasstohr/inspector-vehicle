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
#include "motors/PiMotorControl.h"
#include "uart/UartLineReceiver.h"
#include "stm32h7xx_hal.h"

/* huart3 is defined in main.cpp — declared here so the ISR vector can
 * call HAL_UART_IRQHandler() without a global header for the handle. */
extern UART_HandleTypeDef huart3;

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


/* ── Hardware timer (TIM2) ─────────────────────────────────────────────── */
void TIM2_IRQHandler(void) {
    TIM2->SR &= ~TIM_SR_UIF;   /* clear update interrupt flag               */
    PiMotorControl::isr();
}

/* ── USART3 receive (ST-Link VCP) ───────────────────────────────────────── */
/**
 * Direct UART ISR — bypasses the HAL receive state machine entirely.
 *
 * Root cause of "interrupt never fires" with HAL_UART_Receive_IT:
 *   With FIFO mode enabled HAL enables RXFTIE (threshold interrupt) instead
 *   of RXFNEIE, and any UART error (ORE, FE, NE) diverts to ErrorCallback
 *   without re-arming, killing the receive chain permanently.
 *
 * This handler reads RDR directly when RXNE/RXFNE is set.  Reading RDR
 * automatically clears the flag — no re-arming ever needed.
 * Overrun / framing / noise flags are cleared explicitly so the ISR does
 * not immediately re-fire.
 */
void USART3_IRQHandler(void) {
    if (USART3->ISR & USART_ISR_RXNE_RXFNE) {
        const uint8_t rxByte = static_cast<uint8_t>(USART3->RDR);  /* reading RDR clears RXNE */
        if (UartLineReceiver* const r = UartLineReceiver::instance()) {
            r->handleRxByte(rxByte);
        }
    }
    USART3->ICR = USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NECF;  /* clear errors — write-1-to-clear, always safe */
    HAL_UART_IRQHandler(&huart3);
}

} // extern "C"
