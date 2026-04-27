//
// Created by robotcentral on 4/23/26.
//

#ifndef BUILDRECEIVECOMMAND_H
#define BUILDRECEIVECOMMAND_H

#include <array>
#include <string_view>
#include <span>
#include <cstddef>

#include "HostCommeds.h"


static constexpr std::size_t MAX_COMMAND_TOKENS = 8;
static constexpr std::size_t MAX_COMMAND_LENGTH = 128;

class ReceiveCommand {
public:
    explicit ReceiveCommand(std::string_view received_data);

    [[nodiscard]] HostCommandName command() const;

    /// Supports range-for directly: for (auto arg : cmd.args()) { ... }
    [[nodiscard]] std::span<const std::string_view> args() const;

    [[nodiscard]] bool valid() const;

private:
    std::array<std::string_view, MAX_COMMAND_TOKENS> m_tokens;
    std::size_t m_tokenCount{0};
    HostCommandName m_commandName;
};

#endif //BUILDRECEIVECOMMAND_H
