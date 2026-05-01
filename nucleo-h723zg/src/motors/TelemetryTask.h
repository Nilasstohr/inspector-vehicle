/**
 * @file    TelemetryTask.h
 * @brief   Low-priority FreeRTOS task that periodically logs controller
 *          state over UART without ever blocking the controller task.
 *
 * Problem: HAL_UART_Transmit() is blocking.  At 115200 baud a 33-char string
 * takes ~3 ms.  The controller runs every 100 µs — calling logf() from inside
 * the controller loop stalls ~30 consecutive control ticks and lets the PI
 * integrator wind up, causing violent saturation.
 *
 * Solution: all UART I/O is confined here.  The controller task never touches
 * the UART directly.  On a single-core Cortex-M7 under FreeRTOS, the
 * telemetry task and the controller task cannot execute simultaneously, so
 * reading the controller's public double fields here is free of data races.
 */

#pragma once

#include <cstdint>
#include <FreeTOSTask.h>
#include <uart/Uart.h>
#include "ControllerFreeTOSTask.h"

class TelemetryTask final : public FreeTOSTask {
public:
    /**
     * @param uart         UART logger (shared with other tasks — mutex inside).
     * @param controller   The PI controller task whose state we report.
     * @param periodMs     How often to emit a log line (ms).  Default 200 ms
     *                     gives 5 lines/s — readable on a terminal without
     *                     flooding.  Increase if output is too fast.
     */
    TelemetryTask(Uart& uart,
                  const ControllerFreeTOSTask& controller,
                  uint32_t periodMs = 200U);  /* 5 Hz — tune to taste */

protected:
    void run() override;

private:
    Uart&                        m_uart;
    const ControllerFreeTOSTask& m_controller;
    const uint32_t               m_periodMs;
};


