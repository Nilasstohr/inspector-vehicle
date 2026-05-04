//
// Created by robotcentral on 4/30/26.
//

#include "ControllerFreeTOSTask.h"

#include <uart/Uart.h>
#include <uart/UartTransceiver.h>
#include "utils/EncoderMath.h"

ControllerFreeTOSTask::ControllerFreeTOSTask(Uart& uart, DataSampleTimer & data_sample_timer,
    const MotorDriver & motor1_driver, const MotorDriver & motor2_driver)
: m_uart(uart), m_data_sample_timer(data_sample_timer),
m_delta_us_filter_left(
    TransposedIIRFilter(
    VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0,
    VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1,
    VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1
    )
),
m_delta_us_filter_right(
    TransposedIIRFilter(
    VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0,
    VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1,
    VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1
    )
),
m_pi_control_filter_left(
    TransposedIIRFilter(
    VEHICLE_PI_CONTROL_COEFFICIENT_B0,
    VEHICLE_PI_CONTROL_COEFFICIENT_B1,
    VEHICLE_PI_CONTROL_COEFFICIENT_A1
    )
),
m_pi_control_filter_right(
    TransposedIIRFilter(
    VEHICLE_PI_CONTROL_COEFFICIENT_B0,
    VEHICLE_PI_CONTROL_COEFFICIENT_B1,
    VEHICLE_PI_CONTROL_COEFFICIENT_A1
    )
)
,m_motor1_driver(motor1_driver), m_motor2_driver(motor2_driver)
{
    minimumOutput = VEHICLE_MOTOR_DRIVER_PWM_MIN;
    maximumOutput = VEHICLE_MOTOR_DRIVER_PWM_MAX;
}

void ControllerFreeTOSTask::run() {
    /* Register this task so the TIM2 ISR wakes us after every sample tick
     * instead of polling with vTaskDelay. Must be called in run() because
     * xTaskGetCurrentTaskHandle() is only valid once the task has started. */
    m_data_sample_timer.setNotifyTask(xTaskGetCurrentTaskHandle());

    m_ref_w = 0;  /* default to stopped — update with setRefW() from other tasks */

    for (;;) {
        /* Block indefinitely until the ISR calls vTaskNotifyGiveFromISR().
         * pdTRUE clears the notification count so we never consume stale
         * notifications from previous ticks.
         * vTaskNotifyGiveFromISR acts as a full memory barrier — all ISR
         * writes to the DataSampleTimer snapshot fields are visible here. */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /* Snapshot the raw 32-bit integers published by the DataSampleTimer ISR.
         * Each getter is a single LDR — no critical section needed. */
        const int32_t  leftCount    = m_data_sample_timer.getLeftCount();
        const uint32_t leftDeltaUs  = m_data_sample_timer.getLeftTickDeltaUs();
        const int32_t  rightCount   = m_data_sample_timer.getRightCount();
        const uint32_t rightDeltaUs = m_data_sample_timer.getRightTickDeltaUs();

        /* All float / double maths happens here in task context — never in ISR. */
        m_left_wheel_distance  = static_cast<float>(leftCount)  * Encoder::kCountsToCentiMeters;
        m_right_wheel_distance = static_cast<float>(rightCount) * Encoder::kCountsToCentiMeters;

        m_delta_us_filter_left.update(leftDeltaUs);
        m_delta_us_filter_right.update(rightDeltaUs);

        m_left_read_w  = EncoderMath::angularVelocity(m_delta_us_filter_left.get(),  Encoder::kCountsPerRev);
        m_right_read_w = EncoderMath::angularVelocity(m_delta_us_filter_right.get(), Encoder::kCountsPerRev);
        // update controller for left
        if (isinf(m_left_read_w)) {
            m_left_read_w = 0;
        }
        wLeft = m_pi_control_filter_left.update(m_ref_w - m_left_read_w);
        if (wLeft < minimumOutput) {
            wLeft = minimumOutput;
            m_pi_control_filter_left.resetToOutput(wLeft);   /* anti-windup */
        } else if (wLeft > maximumOutput) {
            wLeft = maximumOutput;
            m_pi_control_filter_left.resetToOutput(wLeft);   /* anti-windup */
        }
        // update controller for right
        if (isinf(m_right_read_w)) {
            m_right_read_w = 0;
        }
        wRight = m_pi_control_filter_right.update(m_ref_w - m_right_read_w);
        if (wRight < minimumOutput) {
            wRight = minimumOutput;
            m_pi_control_filter_right.resetToOutput(wRight); /* anti-windup */
        } else if (wRight > maximumOutput) {
            wRight = maximumOutput;
            m_pi_control_filter_right.resetToOutput(wRight); /* anti-windup */
        }
        /* Drive motors with the controller output.
         * wLeft/wRight are clamped to [minimumOutput, maximumOutput] = [100, 3199]
         * which fits safely in uint16_t (max 65535). */
        m_motor1_driver.setMotorPwm(static_cast<uint16_t>(wLeft));
        m_motor2_driver.setMotorPwm(static_cast<uint16_t>(wRight));
        /* Telemetry (UART logging) is handled by TelemetryTask at low priority.
         * Never log directly from this loop — HAL_UART_Transmit() is blocking
         * and would stall ~30 control ticks per call at 115200 baud, causing
         * integrator wind-up and saturation. */
    }
}
