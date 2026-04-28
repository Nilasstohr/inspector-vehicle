/**
 * @file  DwtTimer.h
 * @brief Microsecond timestamp using the ARM DWT cycle counter.
 *
 * Zero peripheral cost — reuses the debug cycle counter already present on
 * all Cortex-M7 devices.
 *
 * Call DwtTimer::init() once before use (e.g. at the top of main()).
 *
 * Resolution : 1 µs  (at 64 MHz: 15.6 ns raw, divided to µs)
 * Wrap period: ~71 minutes (uint32_t @ 1 MHz effective tick)
 */
#pragma once

#include <cstdint>
#include "stm32h7xx_hal.h"   /* CoreDebug, DWT, SystemCoreClock */

class DwtTimer {
public:
    /** Enable the DWT cycle counter. Must be called once before getMicros(). */
    static void init() {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  /* enable trace */
        DWT->CYCCNT       = 0U;
        DWT->CTRL        |= DWT_CTRL_CYCCNTENA_Msk;       /* start counter */
    }

    /**
     * @return Current timestamp in microseconds.
     *         Wraps after ~71 minutes — subtraction still gives correct dt
     *         across the wrap (unsigned arithmetic).
     */
    static uint32_t getMicros() {
        /* cycles ÷ (cycles/µs) — integer divide, negligible cost in ISR */
        return DWT->CYCCNT / (SystemCoreClock / 1'000'000U);
    }

private:
    DwtTimer() = delete;  /* static-only utility */
};

