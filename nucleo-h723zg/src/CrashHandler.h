#pragma once

#include <stdint.h>

/**
 * @file CrashHandler.h
 * @brief Production fault capture: saves register state to .noinit RAM
 *        (survives soft reset) and optionally prints over UART.
 *
 * Two-layer strategy:
 *  1. Immediate  — UART dump before halting (visible if terminal is open)
 *  2. Persistent — crash record in .noinit RAM, logged on next boot via
 *                  CrashHandler_checkAndReport()
 *
 * Usage in main():
 *   CrashHandler_checkAndReport(&logger);   // call before vTaskStartScheduler
 */

#ifdef __cplusplus
extern "C" {
#endif

/** CPU register frame pushed automatically by Cortex-M on exception entry. */
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;   /**< Link Register — last known caller */
    uint32_t pc;   /**< Program Counter — address that faulted */
    uint32_t xpsr;
} CpuFrame_t;

/** Persists across soft resets in .noinit RAM. */
typedef struct {
    uint32_t   magic;      /**< 0xDEADBEEF when record is valid */
    CpuFrame_t frame;
    uint32_t   hfsr;       /**< HardFault Status Register  */
    uint32_t   cfsr;       /**< Configurable Fault SR      */
    uint32_t   mmfar;      /**< MemManage fault address    */
    uint32_t   bfar;       /**< BusFault address           */
} CrashRecord_t;

#define CRASH_MAGIC  0xDEADBEEFu

/**
 * Called from the naked HardFault_Handler trampoline.
 * Saves registers, prints over UART (raw HAL — no FreeRTOS), then halts.
 */
void HardFault_HandlerC(uint32_t* frame);

#ifdef __cplusplus
}
#endif

/* C++ helper — call once at boot to log any saved crash record */
#ifdef __cplusplus
#include "uart/Uart.h"
void CrashHandler_checkAndReport(Uart* logger);
#endif

