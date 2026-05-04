//
// Created by robotcentral on 4/30/26.
//

#ifndef SAMPLETIMER_H
#define SAMPLETIMER_H

#include <cstdint>
#include <gpio/GpioOutput.h>

#include "Encoder.h"
#include "MotorDriver.h"
#include "TransposedIIRFilter.h"

/**
 * @class DataSampleTimer
 * @brief Receives 100 µs ticks from TIM2 and runs the full PI controller loop
 *        directly in ISR context.
 *
 * Mirrors the Encoder pattern:
 *   - registerInstance() wires the object to the static ISR trampoline.
 *   - isr() is called directly from TIM2_IRQHandler in isr_handlers.cpp.
 *   - All sensor filtering, angular-velocity computation, PI control, and
 *     motor PWM updates happen inside handleTick().
 *
 * Design rationale
 * ────────────────
 * Measured execution time of the controller loop is ~17 µs and the ISR tick
 * period is 100 µs, leaving ~83 µs of headroom.  The previous design offloaded
 * the controller to a FreeRTOS task, but the vTaskNotifyGiveFromISR() overhead
 * alone consumed ~18 µs, giving no latency benefit while adding scheduling
 * jitter.  Moving the controller directly into the ISR eliminates that overhead
 * and ensures deterministic, zero-jitter execution every 100 µs.
 *
 * m_left_read_w / m_right_read_w are public so TelemetryTask can snapshot them
 * between ticks for UART logging.  On a single-core Cortex-M7 each is a 64-bit
 * double written/read as two 32-bit stores/loads; TelemetryTask reads them only
 * while preempted (not inside a critical section), so occasional tearing is
 * acceptable for diagnostic telemetry.
 */
class DataSampleTimer {
public:
    DataSampleTimer(const Encoder &motor1_encoder, const Encoder &motor2_encoder, const MotorDriver &motor1_driver,
                    const MotorDriver &motor2_driver, const GpioOutput &timing_test_pin);

    DataSampleTimer(const DataSampleTimer&)            = delete;
    DataSampleTimer& operator=(const DataSampleTimer&) = delete;
    DataSampleTimer(DataSampleTimer&&)                 = delete;
    DataSampleTimer& operator=(DataSampleTimer&&)      = delete;

    /** Wire this instance to the ISR trampoline — call before start(). */
    static void registerInstance(DataSampleTimer& inst);

    /** Called by TIM2_IRQHandler — do not call from application code. */
    static void isr();

    /** @return Total tick count (each tick = 100 µs). */
    [[nodiscard]] uint32_t tickCount() const;

    /**
     * @brief Measured angular velocities — written by ISR, read by TelemetryTask.
     *        Acceptable for diagnostic use; see class-level note on tearing.
     */
    double m_left_read_w  {0.0};
    double m_right_read_w {0.0};

    /**
     * @brief Current reference angular velocity (rad/s).
     *        Write from a task (or main) before the scheduler starts.
     *        ISR reads this every tick — write is a single 64-bit store on
     *        Cortex-M7, acceptable for this use case.
     */
    double m_ref_w {0};

private:
    const Encoder& m_encoder1;
    const Encoder& m_encoder2;

    float  m_left_wheel_distance  {0.0F};
    float  m_right_wheel_distance {0.0F};
    double wLeft  {0.0};
    double wRight {0.0};
    int    m_minimumOutput {0};
    int    m_maximumOutput {0};
    TransposedIIRFilter m_delta_us_filter_left;
    TransposedIIRFilter m_delta_us_filter_right;
    TransposedIIRFilter m_pi_control_filter_left;
    TransposedIIRFilter m_pi_control_filter_right;
    const MotorDriver & m_motor1_driver;
    const MotorDriver & m_motor2_driver;
    const GpioOutput &  m_timing_test_pin;

    void handleTick();   /* actual per-tick logic — runs in ISR context */
    static DataSampleTimer* s_instance;
};

#endif //SAMPLETIMER_H
