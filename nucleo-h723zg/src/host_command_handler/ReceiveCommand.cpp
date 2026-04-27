//
// Created by robotcentral on 4/23/26.

#include "ReceiveCommand.h"
#include <algorithm>

#include "HostCommeds.h"
/**
 *
 * @param received_data
 */
ReceiveCommand::ReceiveCommand(std::string_view received_data): m_commandName(HostCommandName::Unknown) {
    if(received_data.back() != ';') {
        return;
    }
    received_data.remove_suffix(1);  // strip the trailing ';'
    while (!received_data.empty() && m_tokenCount < MAX_COMMAND_TOKENS) {
        const auto start = received_data.find_first_not_of(' ');
        if (start == std::string_view::npos)
            break;
        received_data.remove_prefix(start);
        const auto end = received_data.find_first_of(' ');
        m_tokens[m_tokenCount++] = received_data.substr(0, end);
        if (end == std::string_view::npos) break;
        received_data.remove_prefix(end);
    }
    if (m_tokenCount > 0) {
        const auto cmdOpt = toHostCommand(m_tokens[0]);
        m_commandName = cmdOpt.value_or(HostCommandName::Unknown);
    }

}

HostCommandName ReceiveCommand::command() const {
    return m_commandName;
}

std::span<const std::string_view> ReceiveCommand::args() const {
    if (!valid()) return {};
    // Skip the first token (command verb), return the rest as args
    const std::string_view* argsBegin = m_tokens.data() + 1;
    const std::size_t argsCount = m_tokenCount - 1;
    return {argsBegin, argsCount};
}

bool ReceiveCommand::valid() const {
    return m_tokenCount > 0 && m_commandName != HostCommandName::Unknown;
}
