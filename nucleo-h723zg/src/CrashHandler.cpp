#include "CrashHandler.h"
#include "stm32h7xx_hal.h"
#include <string.h>
#include <stdio.h>

/* ── Fault Status Register addresses (Cortex-M7) ──────────────────────── */
#define REG_HFSR   (*((volatile uint32_t*)0xE000ED2Cu))
#define REG_CFSR   (*((volatile uint32_t*)0xE000ED28u))
#define REG_MMFAR  (*((volatile uint32_t*)0xE000ED34u))
#define REG_BFAR   (*((volatile uint32_t*)0xE000ED38u))

/**
 * Placed in .noinit so the linker/startup never zeroes this region.
 * The record survives a soft reset (power-on reset clears it naturally).
 */
__attribute__((section(".noinit")))
static volatile CrashRecord_t s_crash;

/* ── Forward declaration of UART used for immediate dump ──────────────── */
extern UART_HandleTypeDef huart3;  /* defined in main.cpp */

static void uart_puts(const char* s) {
    HAL_UART_Transmit(&huart3, (const uint8_t*)s, (uint16_t)strlen(s), 200);
}

static void uart_hex32(const char* label, uint32_t val) {
    char buf[48];
    snprintf(buf, sizeof(buf), "  %-8s= 0x%08lX\r\n", label, (unsigned long)val);
    uart_puts(buf);
}

/* ── HardFault_HandlerC ─────────────────────────────────────────────────
 * Called from the naked trampoline in main.cpp.
 * Runs with interrupts still enabled enough to use HAL_UART_Transmit.     */
__attribute__((used))
void HardFault_HandlerC(uint32_t* frame)
{
    /* 1. Save to .noinit — survives the upcoming reset */
    s_crash.magic     = CRASH_MAGIC;
    s_crash.frame.r0  = frame[0];
    s_crash.frame.r1  = frame[1];
    s_crash.frame.r2  = frame[2];
    s_crash.frame.r3  = frame[3];
    s_crash.frame.r12 = frame[4];
    s_crash.frame.lr  = frame[5];
    s_crash.frame.pc  = frame[6];
    s_crash.frame.xpsr= frame[7];
    s_crash.hfsr      = REG_HFSR;
    s_crash.cfsr      = REG_CFSR;
    s_crash.mmfar     = REG_MMFAR;
    s_crash.bfar      = REG_BFAR;

    /* 2. Immediate UART dump (best-effort — UART may itself be broken) */
    uart_puts("\r\n\r\n*** HARD FAULT ***\r\n");
    uart_hex32("PC",    s_crash.frame.pc);
    uart_hex32("LR",    s_crash.frame.lr);
    uart_hex32("R0",    s_crash.frame.r0);
    uart_hex32("R1",    s_crash.frame.r1);
    uart_hex32("R2",    s_crash.frame.r2);
    uart_hex32("R3",    s_crash.frame.r3);
    uart_hex32("R12",   s_crash.frame.r12);
    uart_hex32("xPSR",  s_crash.frame.xpsr);
    uart_puts("-- Fault Status --\r\n");
    uart_hex32("HFSR",  s_crash.hfsr);
    uart_hex32("CFSR",  s_crash.cfsr);
    uart_hex32("MMFAR", s_crash.mmfar);
    uart_hex32("BFAR",  s_crash.bfar);
    char decode_buf[80];
    snprintf(decode_buf, sizeof(decode_buf),
             "Decode: arm-none-eabi-addr2line -e nucleo-h723zg.elf -f -p -i 0x%08lX\r\n",
             (unsigned long)s_crash.frame.pc);
    uart_puts(decode_buf);

    __disable_irq();
    while (1);  /* Attach GDB here: frame pointer still valid */
}

/* ── CrashHandler_checkAndReport ────────────────────────────────────────
 * Call once at boot (before scheduler). Logs any crash saved from a
 * previous run and then clears the record.                                */
void CrashHandler_checkAndReport(Uart* logger)
{
    if (s_crash.magic != CRASH_MAGIC) return;

    logger->logf("[crash] *** Previous run faulted! ***\r\n");
    logger->logf("[crash]   PC   = 0x%08lX\r\n", (unsigned long)s_crash.frame.pc);
    logger->logf("[crash]   LR   = 0x%08lX\r\n", (unsigned long)s_crash.frame.lr);
    logger->logf("[crash]   HFSR = 0x%08lX\r\n", (unsigned long)s_crash.hfsr);
    logger->logf("[crash]   CFSR = 0x%08lX\r\n", (unsigned long)s_crash.cfsr);

    /* Decode CFSR bits into human-readable fault reason */
    uint32_t cfsr = s_crash.cfsr;
    if (cfsr & (1u << 16)) logger->logf("[crash]   CFSR: IBUSERR  - instruction bus error\r\n");
    if (cfsr & (1u << 17)) logger->logf("[crash]   CFSR: PRECISERR - precise data bus error @ BFAR\r\n");
    if (cfsr & (1u << 18)) logger->logf("[crash]   CFSR: IMPRECISERR - imprecise data bus error\r\n");
    if (cfsr & (1u << 19)) logger->logf("[crash]   CFSR: UNSTKERR - bus fault on exception return\r\n");
    if (cfsr & (1u << 20)) logger->logf("[crash]   CFSR: STKERR   - bus fault on exception entry\r\n");
    if (cfsr & (1u << 24)) logger->logf("[crash]   CFSR: UNDEFINSTR - undefined instruction\r\n");
    if (cfsr & (1u << 25)) logger->logf("[crash]   CFSR: INVSTATE  - invalid EPSR state\r\n");
    if (cfsr & (1u << 26)) logger->logf("[crash]   CFSR: INVPC    - invalid PC load\r\n");
    if (cfsr & (1u << 27)) logger->logf("[crash]   CFSR: NOCP     - no coprocessor\r\n");
    if (cfsr & (1u << 0))  logger->logf("[crash]   CFSR: IACCVIOL - MPU instruction access violation\r\n");
    if (cfsr & (1u << 1))  logger->logf("[crash]   CFSR: DACCVIOL - MPU data access violation\r\n");

    if (s_crash.cfsr & (1u << 15)) /* BFARVALID */
        logger->logf("[crash]   BFAR  = 0x%08lX\r\n", (unsigned long)s_crash.bfar);
    if (s_crash.cfsr & (1u << 7))  /* MMARVALID */
        logger->logf("[crash]   MMFAR = 0x%08lX\r\n", (unsigned long)s_crash.mmfar);
    logger->logf("[crash] Run on host: arm-none-eabi-addr2line -e cmake-build-debug/nucleo-h723zg.elf -f -p -i 0x%08lX\r\n",
                 (unsigned long)s_crash.frame.pc);

    /* Clear so we don't re-report on the next boot */
    s_crash.magic = 0;
}

