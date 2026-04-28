/**
 * @file  test_EncoderMath.cpp
 * @brief Unit tests for EncoderMath::deltaUsToCountsPerSecond.
 */

#include <EncoderMath.h>
#include <gtest/gtest.h>
#include "EncoderMath.h"

using EncoderMath::deltaUsToCountsPerSecond;

TEST(EncoderMath, ZeroDeltaReturnsZero) {
    EXPECT_EQ(deltaUsToCountsPerSecond(0U), 0);
}

TEST(EncoderMath, OneEdgePerSecond) {
    // 1 count in 1 000 000 µs → 1 cps
    EXPECT_EQ(deltaUsToCountsPerSecond(1'000'000U), 1);
}

TEST(EncoderMath, TenEdgesPerSecond) {
    // 1 count in 100 000 µs → 10 cps
    EXPECT_EQ(deltaUsToCountsPerSecond(100'000U), 10);
}

TEST(EncoderMath, MaxMotorSpeed) {
    // 200 rpm, 3200 CPR → 10 666 ticks/s → dt ≈ 93.75 µs
    // Expected: 1'000'000 / 93 = 10752  (integer truncation)
    constexpr uint32_t dtUs = 93U;
    EXPECT_EQ(deltaUsToCountsPerSecond(dtUs), 1'000'000 / 93);
}

TEST(EncoderMath, Oneмикросекунда) {
    // 1 µs between edges → 1 000 000 cps (theoretical max)
    EXPECT_EQ(deltaUsToCountsPerSecond(1U), 1'000'000);
}

TEST(EncoderMath, calculateMaxMotorSpeed) {
    constexpr uint16_t countsPerRev = 3200;
    constexpr uint32_t dtUs = 94U;
    EXPECT_NEAR(EncoderMath::angularVelocity(dtUs,countsPerRev), 20.8882504, 1e-3);
}


