//
// Created by robotcentral on 4/24/26.
//

#ifndef VELOCITYCOMMAND_H
#define VELOCITYCOMMAND_H
#include <span>
#include <string_view>

class VelocityCommand {
public:
    explicit VelocityCommand(std::span<const std::string_view> span);

    [[nodiscard]] bool valid() const;

    [[nodiscard]] float left()  const { return m_left; }
    [[nodiscard]] float right() const { return m_right; }

private:
    float m_left  = 0.0f;
    float m_right = 0.0f;
    bool  m_valid = false;
};

#endif //VELOCITYCOMMAND_H
