/**
 * @file    stm32h7xx_hal_conf.h
 * @brief   HAL configuration for STM32H723ZG
 *
 * =============================================================================
 * ORIGIN
 * =============================================================================
 * This file is a manually stripped-down version of the template ST provides
 * via STM32CubeIDE / CubeMX for the STM32H7 series.
 *
 * Official full template reference:
 *   https://github.com/STMicroelectronics/STM32CubeH7/blob/master/
 *   Projects/NUCLEO-H723ZG/Templates/Inc/stm32h7xx_hal_conf.h
 *
 * The original template enables ALL HAL modules. We stripped it down to only
 * what is needed for this project to keep compile times short and binary
 * size small.
 *
 * =============================================================================
 * HOW IT WORKS
 * =============================================================================
 * The STM32H7 HAL uses a guard-based system:
 *   #ifdef HAL_GPIO_MODULE_ENABLED
 *     #include "stm32h7xx_hal_gpio.h"
 *   #endif
 *
 * By only defining the modules you need, you prevent unused HAL drivers from
 * being compiled. Each enabled module also requires its .c source file to be
 * listed in CMakeLists.txt.
 *
 * =============================================================================
 * DESIGN DECISIONS
 * =============================================================================
 * - USE_RTOS = 0:
 *     The STM32H7 HAL v1.11.x checks this flag and throws a compile error if
 *     set to 1, because HAL_Delay() uses blocking waits that are not RTOS-safe.
 *     Setting it to 0 tells the HAL to use its built-in blocking wait.
 *     This does NOT mean we are not using FreeRTOS — it just means we let HAL
 *     use its own simple blocking mechanism for short waits (flash writes etc).
 *     Reference: stm32h7xx_hal_def.h line ~91.
 *
 * - CSI_VALUE = 4000000U:
 *     The CSI (Clock Security System oscillator) runs at 4MHz on STM32H7.
 *     The HAL RCC driver references this value internally even if you do not
 *     use CSI. Without it, the HAL_RCC_OscConfig() and clock frequency
 *     calculation functions fail to compile with "undeclared identifier" errors.
 *     Source: STM32H723 Reference Manual RM0468, Section 8 (RCC).
 *
 * - EXTERNAL_CLOCK_VALUE = 12288000U:
 *     Used internally by HAL_RCCEx_GetD1PCLK1Freq() and related functions
 *     to calculate peripheral clock frequencies for I2S/SAI external clocks.
 *     We do not use I2S/SAI in this project but the HAL RCC_ex driver still
 *     references this symbol, so it must be defined.
 *     Value of 12.288 MHz is the typical I2S audio clock standard (chosen
 *     as a sensible default; adjust if you use external I2S).
 *
 * - TICK_INT_PRIORITY = 0x0F (lowest):
 *     The SysTick interrupt must have the LOWEST NVIC priority so that
 *     FreeRTOS can safely mask it during critical sections.
 *     FreeRTOS masks interrupts at or below configMAX_SYSCALL_INTERRUPT_PRIORITY.
 *     Since SysTick drives both HAL_Delay() and FreeRTOS tick, it must be
 *     maskable. Priority 15 = lowest on STM32H7 (4 priority bits, 0=highest).
 *
 * - assert_param = no-op:
 *     ST's HAL uses assert_param() throughout for parameter validation.
 *     Defining it as ((void)0U) disables all parameter checks, which saves
 *     code space and speed in production. For debugging HAL issues, you can
 *     re-enable it by defining a real assert handler.
 * =============================================================================
 */

#ifndef __STM32H7xx_HAL_CONF_H
#define __STM32H7xx_HAL_CONF_H

/* ── HAL modules to compile ─────────────────────────────────────────────────
 * Only enable what you use. For each module enabled here, you MUST also add
 * the corresponding .c file in CMakeLists.txt under add_executable().
 *
 * To add a new peripheral (e.g. UART):
 *   1. Add:  #define HAL_UART_MODULE_ENABLED
 *   2. Add:  #include "stm32h7xx_hal_uart.h"  (at the bottom of this file)
 *   3. In CMakeLists.txt add:
 *            ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_uart.c
 *            ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_uart_ex.c
 * ─────────────────────────────────────────────────────────────────────────── */
#define HAL_MODULE_ENABLED         /* Always required — base HAL init          */
#define HAL_GPIO_MODULE_ENABLED    /* Digital I/O — LED, button, any GPIO pin  */
#define HAL_RCC_MODULE_ENABLED     /* Clock control — required for SystemClock_Config() */
#define HAL_CORTEX_MODULE_ENABLED  /* SysTick, NVIC, MPU — required by HAL_Init() */
#define HAL_PWR_MODULE_ENABLED     /* Power control — voltage scaling, sleep modes */
#define HAL_FLASH_MODULE_ENABLED   /* Flash access — required internally by HAL_RCC_ClockConfig() */
#define HAL_DMA_MODULE_ENABLED     /* DMA types — required by UART handle struct  */
#define HAL_UART_MODULE_ENABLED    /* UART/USART — debug logging via ST-Link VCP  */

/* ── Oscillator frequency values ────────────────────────────────────────────
 * These must match the actual hardware. Used by HAL RCC functions to
 * calculate bus/peripheral clock frequencies.
 * Source: Nucleo-H723ZG schematic (MB1364) + STM32H723 datasheet DS13312.
 * ─────────────────────────────────────────────────────────────────────────── */
#define HSE_VALUE            8000000U   /* External oscillator: 8MHz from ST-Link MCO output   */
#define HSI_VALUE           64000000U   /* Internal RC oscillator: 64MHz (default after reset) */
#define CSI_VALUE            4000000U   /* Low-power internal RC: 4MHz (referenced by HAL RCC) */
#define LSE_VALUE               32768U  /* Low-speed external crystal: 32.768kHz (RTC)         */
#define HSI48_VALUE         48000000U   /* 48MHz RC for USB (not used here)                    */
#define EXTERNAL_CLOCK_VALUE 12288000U  /* I2S/SAI external clock default (not used here)      */

#define HSE_STARTUP_TIMEOUT     100U   /* ms to wait for HSE to stabilise before giving up */
#define LSE_STARTUP_TIMEOUT    5000U   /* ms to wait for LSE (crystal is slow to start)    */

/* ── Misc HAL settings ──────────────────────────────────────────────────────
 * ─────────────────────────────────────────────────────────────────────────── */
#define VDD_VALUE               3300U  /* Supply voltage in mV (3.3V on Nucleo)             */
#define TICK_INT_PRIORITY      (0x0FUL) /* SysTick NVIC priority — MUST be lowest (15)      */
#define USE_RTOS                    0  /* Must be 0: HAL v1.11.x rejects 1 with #error      */
#define PREFETCH_ENABLE             1  /* Enable flash prefetch buffer for better performance*/

/* ── HAL module header includes ─────────────────────────────────────────────
 * HAL includes are conditional — only pulled in if the module is enabled above.
 * ─────────────────────────────────────────────────────────────────────────── */
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_cortex.h"
#include "stm32h7xx_hal_pwr.h"
#include "stm32h7xx_hal_flash.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_uart.h"

/* ── Parameter assertion — disabled for production ──────────────────────────
 * HAL uses assert_param() throughout to validate arguments.
 * Defining it as a no-op disables all checks (saves space + speed).
 * To re-enable for debugging: define a real handler that logs or halts.
 * ─────────────────────────────────────────────────────────────────────────── */
#define assert_param(expr)  ((void)0U)

#endif /* __STM32H7xx_HAL_CONF_H */

