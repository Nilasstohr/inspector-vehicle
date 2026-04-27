//
// Created by robotcentral on 4/28/26.
//

#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
#include <gpio/GpioOutput.h>
#include <pwm/PwmOutput.h>

class MotorDriver {
    public:
        MotorDriver(PwmOutput &pwm_output, const GpioOutput &gpio_in_a, const GpioOutput &gpio_in_b);
        void setMotorPwm(uint16_t pwm) const;

        void setForward() const;

        void setReverse() const;

        void stop() const;

    private:
        PwmOutput  & m_pwm_output;
        const GpioOutput & m_gpio_in_a;
        const GpioOutput & m_gpio_in_b;
};

#endif //MOTORDRIVER_H
