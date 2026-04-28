//
// Created by robotcentral on 4/28/26.
//

#include "MotorDriver.h"

MotorDriver::MotorDriver(PwmOutput &pwm_output, const GpioOutput &gpio_in_a, const GpioOutput &gpio_in_b):
m_pwm_output(pwm_output),m_gpio_in_a(gpio_in_a),m_gpio_in_b(gpio_in_b) {}

void MotorDriver::setMotorPwm(const uint16_t pwm) const {
    m_pwm_output.setPwmRawValue(pwm);
}

void MotorDriver::setForward() const {
    m_gpio_in_a.setLow();
    m_gpio_in_b.setHigh();
}

void MotorDriver::setReverse() const {
    m_gpio_in_a.setHigh();
    m_gpio_in_b.setLow();
}

void MotorDriver::stop() const {
    m_pwm_output.setPwmRawValue(0);
}