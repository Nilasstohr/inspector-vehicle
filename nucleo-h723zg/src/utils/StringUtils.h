//
// Stateless string utility helpers — no HAL, no FreeRTOS, no heap.
// Safe for both embedded and host (unit-test) builds.
//
#pragma once
#include <string_view>
#include <cstdlib>
#include <optional>

namespace utils {
    /**
     * Parse a float from a string_view.
     * @return The parsed value, or std::nullopt if the input is not a valid number.
     */
    inline std::optional<float> parseFloat(std::string_view sv) {
        char buf[32] = {};
        if (sv.empty() || sv.size() >= sizeof(buf)) return std::nullopt;
        sv.copy(buf, sv.size());   // zero-init guarantees null termination
        char* end = nullptr;
        const float result = std::strtof(buf, &end);
        if (end == buf) return std::nullopt;  // no digits consumed
        return result;
    }

}
