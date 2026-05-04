/**
 * @file  EncoderMath.h
 * @brief Pure mathematical helpers for quadrature encoder calculations.
 *
 * No hardware dependencies — fully testable on the host.
 */
#pragma once

#include <cstdint>
#include <numbers>   /* std::numbers::pi_v  (C++20) */

namespace EncoderMath {

/**
 * Convert a time-between-edges measurement to a velocity in counts per second.
 *
 * @param dtUs  Elapsed time in **microseconds** between the last two encoder edges.
 *              Pass 0 to indicate no recent edge (returns 0).
 * @return      Velocity magnitude in counts per second (always non-negative).
 *
 * Minimum measurable dt at max motor speed (200 rpm, 3200 CPR):
 *   dt_min = 1 / 10 666 ticks/s × 1 000 000 = ~93.8 µs
 * → 1 µs resolution gives ~1 % error at max speed  ✓
 * → 1 ms resolution (HAL_GetTick) would give 0 or ±1000 cps  ✗
 */
inline int32_t deltaUsToCountsPerSecond(const uint32_t dtUs) {
    if (dtUs == 0U) {
        return 0;
    }
    return static_cast<int32_t>(1'000'000U / dtUs);  /* 1 count per dtUs µs → cps */
}

/**
 * Convert a time-between-edges measurement to angular velocity in radians per second.
 *
 * Derivation:
 *   dx  = 2π / countsPerRev          [rad per count]
 *   dt  = dtUs / 1 000 000           [s]
 *   ω   = dx / dt
 *       = (2π × 1 000 000) / (countsPerRev × dtUs)
 *
 * @param dtUs          Elapsed time in **microseconds** between the last two encoder edges.
 *                      Pass 0 to indicate no recent edge (returns 0.0f).
 * @param countsPerRev  Total encoder counts per full wheel revolution (e.g. 3200).
 * @return              Angular velocity magnitude in **rad/s** (always non-negative).
 */
inline double angularVelocity(const double dtUs, const int32_t countsPerRev) {
    if (dtUs == 0U || countsPerRev == 0) {
        return 0.0;
    }
    /* ω = (2π × 1 000 000) / (countsPerRev × dtUs) */
    constexpr double kTwoPiMicros = 2.0 * std::numbers::pi_v<double> * 1'000'000.0;
    return kTwoPiMicros / (static_cast<double>(countsPerRev) * dtUs);
}

} // namespace EncoderMath

