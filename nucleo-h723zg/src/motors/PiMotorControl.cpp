//
// Created by robotcentral on 4/30/26.
//

#include "PiMotorControl.h"
#include <EncoderMath.h>

/* ── Static registry ──────────────────────────────────────────────────────── */
PiMotorControl* PiMotorControl::s_instance = nullptr;

PiMotorControl::PiMotorControl(const Encoder &motor1_encoder, const Encoder &motor2_encoder,
    const MotorDriver & motor1_driver, const MotorDriver & motor2_driver,const GpioOutput & timing_test_pin):
m_encoder1(motor1_encoder), m_encoder2(motor2_encoder),
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
,m_motor1_driver(motor1_driver), m_motor2_driver(motor2_driver),m_timing_test_pin(timing_test_pin)
{
    m_timing_test_pin.setLow();
    m_minimumOutput = VEHICLE_MOTOR_DRIVER_PWM_MIN;
    m_maximumOutput = VEHICLE_MOTOR_DRIVER_PWM_MAX;
}

void PiMotorControl::setVelocities(const float left, const float right) {
    /* relaxed: single-core Cortex-M7 — no DMB needed, plain STR is sufficient. */
    m_left_ref_w.store(left,   std::memory_order_relaxed);
    m_right_ref_w.store(right, std::memory_order_relaxed);
}

/* ── Registration ─────────────────────────────────────────────────────────── */
void PiMotorControl::registerInstance(PiMotorControl& inst)
{
    s_instance = &inst;
}

/* ── ISR trampoline ───────────────────────────────────────────────────────── */
void PiMotorControl::isr()
{
    if (s_instance != nullptr) {
        s_instance->handleTick();
    }
}

/* ── Per-tick PI controller (ISR context — no blocking calls) ────────────── */
void PiMotorControl::handleTick()
{
    m_timing_test_pin.setHigh();

    /* Snapshot velocity references once — guarantees both wheels use the same
     * command pair for this tick even if setVelocities() is called concurrently. */
    const float leftRefW  = m_left_ref_w.load(std::memory_order_relaxed);
    const float rightRefW = m_right_ref_w.load(std::memory_order_relaxed);

    /* Read raw encoder state — each is a single 32-bit LDR (atomic on Cortex-M7). */
    const int32_t  leftCount   = m_encoder1.getCount();
    const uint32_t leftDeltaUs = m_encoder1.getTickDeltaUs();
    const int32_t  rightCount  = m_encoder2.getCount();
    const uint32_t rightDeltaUs = m_encoder2.getTickDeltaUs();

    /* Wheel distance (cm) — stored for diagnostics / future odometry use. */
    m_left_wheel_distance  = static_cast<float>(leftCount)  * Encoder::kCountsToCentiMeters;
    m_right_wheel_distance = static_cast<float>(rightCount) * Encoder::kCountsToCentiMeters;

    /* Sensor IIR filter → angular velocity estimate. */
    m_delta_us_filter_left.update(leftDeltaUs);
    m_delta_us_filter_right.update(rightDeltaUs);

    m_left_read_w  = EncoderMath::angularVelocity(m_delta_us_filter_left.get(),  Encoder::kCountsPerRev);
    m_right_read_w = EncoderMath::angularVelocity(m_delta_us_filter_right.get(), Encoder::kCountsPerRev);

    /* PI controller — left wheel */
    if (isinf(m_left_read_w)) {
        m_left_read_w = 0.0;
    }
    double wLeft = m_pi_control_filter_left.update(leftRefW - m_left_read_w);
    if (wLeft < m_minimumOutput) {
        wLeft = m_minimumOutput;
        m_pi_control_filter_left.resetToOutput(wLeft);   /* anti-windup */
    } else if (wLeft > m_maximumOutput) {
        wLeft = m_maximumOutput;
        m_pi_control_filter_left.resetToOutput(wLeft);   /* anti-windup */
    }

    /* PI controller — right wheel */
    if (isinf(m_right_read_w)) {
        m_right_read_w = 0.0;
    }
    double wRight = m_pi_control_filter_right.update(rightRefW - m_right_read_w);
    if (wRight < m_minimumOutput) {
        wRight = m_minimumOutput;
        m_pi_control_filter_right.resetToOutput(wRight); /* anti-windup */
    } else if (wRight > m_maximumOutput) {
        wRight = m_maximumOutput;
        m_pi_control_filter_right.resetToOutput(wRight); /* anti-windup */
    }
    m_motor1_driver.setMotorPwm(static_cast<uint16_t>(wLeft));
    m_motor2_driver.setMotorPwm(static_cast<uint16_t>(wRight));

    m_timing_test_pin.setLow();
}
