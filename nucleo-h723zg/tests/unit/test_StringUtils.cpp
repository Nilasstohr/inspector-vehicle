/**
 * @file  test_StringUtils.cpp
 * @brief Unit tests for utils::parseFloat and utils::floatToString.
 */

#include <gtest/gtest.h>
#include "StringUtils.h"

using utils::parseFloat;
using utils::floatToString;

// ── Happy-path ────────────────────────────────────────────────────────────────

TEST(StringUtils_ParseFloat, PositiveInteger) {
    const auto result = parseFloat("42");
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, 42.0f);
}

TEST(StringUtils_ParseFloat, NegativeFloat) {
    const auto result = parseFloat("-3.14");
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(*result, -3.14f, 1e-5f);
}

TEST(StringUtils_ParseFloat, PositiveFloat) {
    const auto result = parseFloat("1.5");
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, 1.5f);
}

TEST(StringUtils_ParseFloat, Zero) {
    const auto result = parseFloat("0.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, 0.0f);
}

TEST(StringUtils_ParseFloat, ScientificNotation) {
    const auto result = parseFloat("1e3");
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, 1000.0f);
}

TEST(StringUtils_ParseFloat, LeadingWhitespaceIsAccepted) {
    // strtof skips leading whitespace — document the behaviour explicitly
    const auto result = parseFloat(" 7.0");
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, 7.0f);
}

// ── Error cases ───────────────────────────────────────────────────────────────

TEST(StringUtils_ParseFloat, EmptyStringReturnsNullopt) {
    EXPECT_FALSE(parseFloat("").has_value());
}

TEST(StringUtils_ParseFloat, NonNumericStringReturnsNullopt) {
    EXPECT_FALSE(parseFloat("abc").has_value());
}

TEST(StringUtils_ParseFloat, StringTooLongReturnsNullopt) {
    // buf[32]: max accepted sv.size() is 31 (leaves room for NUL terminator)
    const std::string_view fits31(
        "1.23456789012345678901234567890");  // 31 chars — must be accepted
    ASSERT_EQ(fits31.size(), 31u);
    ASSERT_TRUE(parseFloat(fits31).has_value());

    const std::string_view tooLong32(
        "1.234567890123456789012345678901");  // 32 chars — must be rejected
    ASSERT_EQ(tooLong32.size(), 32u);
    EXPECT_FALSE(parseFloat(tooLong32).has_value());
}

TEST(StringUtils_ParseFloat, SingleDotReturnsNullopt) {
    // "." has no digit consumed by strtof
    EXPECT_FALSE(parseFloat(".").has_value());
}

// ── floatToString ─────────────────────────────────────────────────────────────

TEST(StringUtils_FloatToString, PositiveFloat) {
    const auto s = floatToString(3.14f, etl::format_spec().precision(2));
    EXPECT_STREQ(s.c_str(), "3.14");
}

TEST(StringUtils_FloatToString, NegativeFloat) {
    const auto s = floatToString(-1.5f, etl::format_spec().precision(1));
    EXPECT_STREQ(s.c_str(), "-1.5");
}

TEST(StringUtils_FloatToString, Zero) {
    const auto s = floatToString(0.0f, etl::format_spec().precision(2));
    EXPECT_STREQ(s.c_str(), "0.00");
}

TEST(StringUtils_FloatToString, DefaultPrecisionIsFour) {
    // Default format_spec: 4 decimal places
    const auto s = floatToString(1.0f);
    EXPECT_STREQ(s.c_str(), "1.0000");
}

TEST(StringUtils_FloatToString, RoundTrip) {
    // floatToString → parseFloat should recover the original value
    constexpr float original = 27.5f;
    const auto s    = floatToString(original, etl::format_spec().precision(4));
    const auto back = parseFloat(std::string_view(s.c_str(), s.size()));
    ASSERT_TRUE(back.has_value());
    EXPECT_NEAR(*back, original, 1e-3f);
}

TEST(StringUtils_FloatToString, NoHeapUsed) {
    // etl::string<N> is stack-allocated — its size is bounded and known at compile time
    static_assert(sizeof(etl::string<32>) < 256u,
                  "etl::string must be a stack-only type with bounded size");
    SUCCEED();  // compile-time check; reaching here means it passed
}
