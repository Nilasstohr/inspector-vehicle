/**
 * @file    Encoder.h
 * @brief   Quadrature encoder with per-instance state and static ISR trampolines.
 *
 * Hardware: Pololu 50:1 37D motor, 64 CPR on motor shaft → 3200 CPR at wheel.
 * Each motor uses two EXTI lines (channel A and channel B) for direction.
 *
 * Pin assignments live in MotorPinConfig.h.
 * ISR wiring in isr_handlers.cpp calls the trampolines directly:
 *   extern "C" void EXTI15_10_IRQHandler() {
 *       if (EXTI_PR1 & MOTOR1_ENC_A_PIN) { EXTI_PR1 = MOTOR1_ENC_A_PIN; Encoder::isrChannelA(0); }
 *       if (EXTI_PR1 & MOTOR1_ENC_B_PIN) { EXTI_PR1 = MOTOR1_ENC_B_PIN; Encoder::isrChannelB(0); }
 *   }
 */

#ifndef ENCODER_H
#define ENCODER_H

#include <cstdint>
#include <numbers>
#include "gpio/GpioInput.h"
#include "VehicleParameters.h"

class Encoder {
public:
    static constexpr uint8_t kMaxInstances        = 2U;    /* left + right */
    static constexpr int32_t kCountsPerRev        = VEHICLE_MOTOR_ENCODER_COUNT_PR_REV;  /* 64 CPR × 50:1 gear ratio */
    static constexpr float   kCountsToCentiMeters = VEHICLE_WHEEL_DIAMETER_CM * std::numbers::pi_v<float> / static_cast<float>(kCountsPerRev);  /* cm/count */
    /**
     * @param pinA  GpioInput wired to encoder channel A (GPIO_MODE_IT_RISING_FALLING)
     * @param pinB  GpioInput wired to encoder channel B (GPIO_MODE_IT_RISING_FALLING)
     */
    Encoder(const GpioInput& pinA, const GpioInput& pinB);

    /* Non-copyable, non-movable — holds hardware references and lives in a static registry */
    Encoder(const Encoder&)            = delete;
    Encoder& operator=(const Encoder&) = delete;
    Encoder(Encoder&&)                 = delete;
    Encoder& operator=(Encoder&&)      = delete;

    /* ── Registration ──────────────────────────────────────────────────── */
    static void registerInstance(uint8_t index, Encoder& inst);

    /* ── ISR trampolines (called from extern "C" ISR stubs) ───────────── */
    static void isrChannelA(uint8_t index);
    static void isrChannelB(uint8_t index);

    /* ── Public API ────────────────────────────────────────────────────── */

    /** Read the accumulated count (32-bit — single LDR, atomically safe). */
    [[nodiscard]] int32_t getCount() const;

    /** Reset count to zero. */
    void resetCount();

    /**
     * Elapsed microseconds between the last two encoder edges (32-bit — single
     * LDR, atomically safe).  Consumed by DataSampleTimer ISR; heavy maths
     * (velocity, distance) are deferred to task context in ControllerFreeRTOSTask.
     */
    [[nodiscard]] uint32_t getTickDeltaUs() const;

private:
    const GpioInput& m_pinA;
    const GpioInput& m_pinB;

    volatile int32_t  m_count      {0};
    /*
     * Tick delta is computed entirely inside the encoder ISR and stored as a
     * single 32-bit word.  A single STR/LDR on Cortex-M7 is atomic, so the
     * lower-priority DataSampleTimer ISR can read m_tickDeltaUs without any
     * critical section — no __disable_irq() / locking needed.
     * m_lastTick is ISR-private scratch; it is never read outside the ISR.
     */
    volatile uint32_t m_lastTick    {0};   /* ISR-private — not read externally */
    volatile uint32_t m_tickDeltaUs {0};   /* atomically consumed by DataSampleTimer ISR */

    uint8_t readPinA() const;
    uint8_t readPinB() const;

    void handleChannelA();
    void handleChannelB();

    static Encoder* s_instances[kMaxInstances];
};

#endif /* ENCODER_H */

