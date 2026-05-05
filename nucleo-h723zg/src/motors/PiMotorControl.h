//
// Created by robotcentral on 4/30/26.
//

#ifndef SAMPLETIMER_H
#define SAMPLETIMER_H

#include <atomic>
#include <cstdint>
#include <gpio/GpioOutput.h>
#include <host_command_handler/VelocityCommand.h>

#include "Encoder.h"
#include "MotorDriver.h"
#include "TransposedIIRFilter.h"

/**
 * @class PiMotorControl
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
class PiMotorControl {
public:
    PiMotorControl( Encoder &motor1_encoder, Encoder &motor2_encoder, const MotorDriver &motor1_driver,
                    const MotorDriver &motor2_driver, const GpioOutput &timing_test_pin);

    PiMotorControl(const PiMotorControl&)            = delete;
    PiMotorControl& operator=(const PiMotorControl&) = delete;
    PiMotorControl(PiMotorControl&&)                 = delete;
    PiMotorControl& operator=(PiMotorControl&&)      = delete;

    [[nodiscard]] float getLeftWheelDistance() const;

    [[nodiscard]] float getRightWheelDistance() const;

    void reset();

    void setVelocities(float left, float right);

    /** Wire this instance to the ISR trampoline — call before start(). */
    static void registerInstance(PiMotorControl& inst);

    /** Called by TIM2_IRQHandler — do not call from application code. */
    static void isr();

    /**
     * @brief Measured angular velocities — written by ISR, read by TelemetryTask.
     *        Acceptable for diagnostic use; see class-level note on tearing.
     */
     volatile double m_left_read_w  {0.0};
     volatile double m_right_read_w {0.0};

    /**
     * @brief Current reference angular velocity (rad/s).
     *        Written by HostCommandHandlerFreeTOSTask via setVelocities(),
     *        read by the TIM2 ISR every tick.
     *
     *        Ordering: memory_order_relaxed is used for all accesses.
     *        On a single-core Cortex-M7 there is no other core to synchronise
     *        with, so seq_cst's DMB barrier buys nothing and costs ~1–2 cycles
     *        per access at 10 kHz.
     *
     *        Pair atomicity: the two stores in setVelocities() are not jointly
     *        atomic — the ISR can theoretically read new_left + old_right on one
     *        tick.  The window is ~2 ns vs a 100 µs period (~0.002 % per call)
     *        and the effect is a single-tick mismatched command, which is
     *        unobservable.  Accepted by design.
     */
    std::atomic<float> m_left_ref_w  {0.0F};
    std::atomic<float> m_right_ref_w {0.0F};

private:
    Encoder& m_encoder1;
    Encoder& m_encoder2;


    std::atomic<float>  m_left_wheel_distance  {0.0F};
    std::atomic<float>  m_right_wheel_distance {0.0F};
    int    m_minimumOutput {0};
    int    m_maximumOutput {0};
    TransposedIIRFilter m_delta_us_filter_left;
    TransposedIIRFilter m_delta_us_filter_right;
    TransposedIIRFilter m_pi_control_filter_left;
    TransposedIIRFilter m_pi_control_filter_right;
    const MotorDriver & m_motor_left_driver;
    const MotorDriver & m_motor_right_driver;
    const GpioOutput &  m_timing_test_pin;

    void handleTick();   /* actual per-tick logic — runs in ISR context */
    static PiMotorControl* s_instance;
};

#endif //SAMPLETIMER_H
