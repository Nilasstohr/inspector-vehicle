/**
 * @file    MotorPinConfig.h
 * @brief   Single source of truth for every motor and encoder GPIO pin assignment.
 *
 * Change a pin here and it propagates automatically to both main.cpp
 * (GpioInput / GpioOutput construction) and isr_handlers.cpp (ISR routing).
 *
 * EXTI line index == pin number (not port).
 *   MOTOR1_ENC_A_PIN → PB1 → EXTI1 → EXTI1_IRQHandler  (dedicated vector)
 *   MOTOR1_ENC_B_PIN → PB2 → EXTI2 → EXTI2_IRQHandler  (dedicated vector)
 */

#pragma once
#include "stm32h7xx_hal.h"

/* ── Motor 1 ──────────────────────────────────────────────────────────────── */
#define MOTOR1_PWM_PORT         GPIOE
#define MOTOR1_PWM_PIN          GPIO_PIN_9    /* TIM1_CH1  — Arduino D6  (CN10) */

#define MOTOR1_INA_PORT         GPIOG
#define MOTOR1_INA_PIN          GPIO_PIN_12   /* Direction A             (CN10) */

#define MOTOR1_INB_PORT         GPIOE
#define MOTOR1_INB_PIN          GPIO_PIN_14   /* Direction B             (CN10) */

#define MOTOR1_ENC_A_PORT       GPIOB
#define MOTOR1_ENC_A_PIN        GPIO_PIN_1    /* Encoder ch A — PB1 → EXTI1 — CN12 */

#define MOTOR1_ENC_B_PORT       GPIOB
#define MOTOR1_ENC_B_PIN        GPIO_PIN_2    /* Encoder ch B — PB2 → EXTI2 — CN12 */

/* Each encoder pin has its own dedicated IRQn — no demux needed. */
#define MOTOR1_ENC_A_IRQn         EXTI1_IRQn
#define MOTOR1_ENC_B_IRQn         EXTI2_IRQn

/* Interrupt priority for encoder EXTI lines.
 *
 * STM32H7 has 4 priority bits → 16 levels, 0 (highest urgency) … 15 (lowest).
 *
 * FreeRTOS divides this range into two zones:
 *   0 … 4  — above configMAX_SYSCALL_INTERRUPT_PRIORITY
 *             FreeRTOS kernel is BLIND to these IRQs; they run at any time,
 *             even inside a critical section.  You MUST NOT call any FreeRTOS
 *             API (not even the "FromISR" variants) from these handlers.
 *
 *   5 … 15 — at or below configMAX_SYSCALL_INTERRUPT_PRIORITY  (= 5 here)
 *             FreeRTOS manages these IRQs.  The "FromISR" API family
 *             (e.g. xTaskNotifyFromISR) is safe to call from these handlers.
 *
 * The encoder ISR currently does NOT call any FreeRTOS API, so it could
 * technically sit anywhere.  It is set to 5 (the boundary) so that:
 *   a) It runs at the highest urgency still visible to FreeRTOS.
 *   b) Future additions (e.g. xTaskNotifyFromISR to wake a control task)
 *      will work without changing the priority.
 *
 * See FreeRTOSConfig.h: configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5
 */
#define MOTOR1_ENC_IRQ_PRIORITY   5U

/* ── Motor 2 ──────────────────────────────────────────────────────────────── */
#define MOTOR2_PWM_PORT         GPIOE
#define MOTOR2_PWM_PIN          GPIO_PIN_11   /* TIM1_CH2  — Arduino D5  (CN10) */

#define MOTOR2_INA_PORT         GPIOE
#define MOTOR2_INA_PIN          GPIO_PIN_13   /* Direction A             (CN10) */

#define MOTOR2_INB_PORT         GPIOG
#define MOTOR2_INB_PIN          GPIO_PIN_14   /* Direction B             (CN10) */
