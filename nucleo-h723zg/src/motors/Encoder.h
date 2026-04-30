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
#include "gpio/GpioInput.h"

class Encoder {
public:
    static constexpr uint8_t kMaxInstances = 2U;    /* left + right */
    static constexpr int32_t kCountsPerRev = 3200;  /* 64 CPR × 50:1 gear ratio */

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

    /** Read the accumulated count. */
    int32_t getCount() const;

    /** Reset count to zero. */
    void resetCount();

    /** Velocity in counts per second — derived from last two edge timestamps. */
    int32_t getVelocityCps() const;

    /** Angular velocity in radians per second — derived from last two edge timestamps. */
    [[nodiscard]] float getAngularVelocityRps() const;

private:
    const GpioInput& m_pinA;
    const GpioInput& m_pinB;

    volatile int32_t  m_count       {0};
    volatile uint32_t m_lastTick    {0};
    volatile uint32_t m_currentTick {0};

    uint8_t readPinA() const;
    uint8_t readPinB() const;

    void handleChannelA();
    void handleChannelB();

    static Encoder* s_instances[kMaxInstances];
};

#endif /* ENCODER_H */

