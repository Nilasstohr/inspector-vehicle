/**
 * @file    TelemetryTask.h
 * @brief   Low-priority FreeRTOS task that periodically logs controller
 *          state over UART without ever blocking the ISR controller loop.
 *
 * Problem: HAL_UART_Transmit() is blocking.  At 115200 baud a 33-char string
 * takes ~3 ms.  The controller ISR runs every 100 µs — calling logf() from
 * inside the ISR would stall ~30 consecutive control ticks and let the PI
 * integrator wind up, causing violent saturation.
 *
 * Solution: all UART I/O is confined here.  The ISR never touches the UART.
 * TelemetryTask snapshots DataSampleTimer's public diagnostic fields
 * (m_left_read_w, m_right_read_w) at low priority for human-readable output.
 */

#pragma once

#include <cstdint>
#include <FreeTOSTask.h>
#include <uart/Uart.h>
#include "PiMotorControl.h"

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
                  const PiMotorControl& data_sample_timer,
                  uint32_t periodMs = 200U);  /* 5 Hz — tune to taste */

protected:
    void run() override;

private:
    Uart&                        m_uart;
    const PiMotorControl& m_data_sample_timer;
    const uint32_t               m_periodMs;
};


