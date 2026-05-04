/**
 * @file    TelemetryTask.cpp
 * @brief   See TelemetryTask.h for design rationale.
 */

#include "TelemetryTask.h"

/* FreeRTOS.h and task.h are pulled in transitively through FreeTOSTask.h /
 * ControllerFreeTOSTask.h, but listed explicitly for clarity. */
#include "FreeRTOS.h"
#include "task.h"

TelemetryTask::TelemetryTask(Uart& uart,
                             const PiMotorControl& data_sample_timer,
                             const uint32_t periodMs)
    : m_uart(uart), m_data_sample_timer(data_sample_timer), m_periodMs(periodMs)
{}

void TelemetryTask::run() {
    /* Convert period once — pdMS_TO_TICKS uses the compile-time tick rate. */
    const TickType_t delay = pdMS_TO_TICKS(m_periodMs);

    for (;;) {
        vTaskDelay(delay);

        /* Snapshot controller state.  On a single-core Cortex-M7 under
         * FreeRTOS, the controller task is preempted while we run here, so
         * reading its public fields is free of concurrent-access races.
         * Cast to float for %f — double printf is heavier and accuracy to
         * 2 decimal places is enough for debugging. */
        //const float refW   = static_cast<float>(m_data_sample_timer.m_ref_w);
        const float leftW  = static_cast<float>(m_data_sample_timer.m_left_read_w);
        const float rightW = static_cast<float>(m_data_sample_timer.m_right_read_w);
        //const float pwmL   = static_cast<float>(m_data_sample_timer.wLeft);
        //const float pwmR   = static_cast<float>(m_data_sample_timer.wRight);
        m_uart.logf("ref:%.2f  lw:%.2f\n\r",leftW,rightW);
    }
}


