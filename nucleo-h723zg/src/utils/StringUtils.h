//
// Stateless string utility helpers — no HAL, no FreeRTOS, no heap.
// Safe for both embedded and host (unit-test) builds.
//
#pragma once
#include <string_view>
#include <cstdlib>
#include <optional>
#include "etl/to_string.h"
#include "etl/string.h"
#include "etl/format_spec.h"

namespace utils {
    /**
     * Parse a float from a string_view.
     * @return The parsed value, or std::nullopt if the input is not a valid number.
     */
    inline std::optional<float> parseFloat(std::string_view sv) {
        char buf[32] = {};
        if ( sv.empty() || sv.size() >= sizeof(buf)) {
            return std::nullopt;
        }
        sv.copy(buf, sv.size());   // zero-init guarantees null termination
        char* end = nullptr;
        const float result = std::strtof(buf, &end);
        if (end == buf)
            return std::nullopt;  // no digits consumed
        return result;
    }

    /**
     * Convert a float to a fixed-capacity stack string using ETL.
     *
     * @tparam N  Buffer capacity in characters (default 32).
     * @param  value  Value to format.
     * @param  fmt    ETL format spec — defaults to 4 decimal places, fixed notation.
     * @return  etl::string<N> on the stack; never allocates heap.
     *
     * Example:
     *   const auto s = utils::floatToString(3.14f);          // "3.1400"
     *   const auto s = utils::floatToString(3.14f,
     *       etl::format_spec().precision(2).fixed());         // "3.14"
     */
    template<size_t N = 32>
    [[nodiscard]]  etl::string<N> floatToString(
        const float value,
        const etl::format_spec& fmt = etl::format_spec().precision(4)) noexcept
    {
        etl::string<N> buf;
        etl::to_string(value, buf, fmt);
        return buf;
    }

}
