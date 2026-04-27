//
// Created by robotcentral on 4/24/26.
//

#include "VelocityCommand.h"
#include "utils/StringUtils.h"

VelocityCommand::VelocityCommand(std::span<const std::string_view> args) {
    if (args.size() < 2) return;

    const auto left  = utils::parseFloat(args[0]);
    const auto right = utils::parseFloat(args[1]);

    if (left.has_value() && right.has_value()) {
        m_left  = left.value();
        m_right = right.value();
        m_valid = true;
    }
}

bool VelocityCommand::valid() const {
    return m_valid;
}
