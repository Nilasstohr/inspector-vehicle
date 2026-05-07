//
// Created by robotcentral on 4/24/26.
//

#ifndef HOSTCOMMEDS_H
#define HOSTCOMMEDS_H

#include <string_view>
#include <optional>

enum class HostCommandName : uint8_t {
    Vel,
    Dis,
    Reset,
    Stop,
    Unknown
};

inline std::optional<HostCommandName> toHostCommand(std::string_view s) {
    if (s == "v") return HostCommandName::Vel;
    if (s == "p") return HostCommandName::Dis;
    if (s == "r") return HostCommandName::Reset;
    if (s =="s")  return HostCommandName::Stop;
    return HostCommandName::Unknown;  // unknown command
}

#endif //HOSTCOMMEDS_H
