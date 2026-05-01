//
// Created by robotcentral on 4/30/26.
//

#ifndef CONTROLLERFREETOSTASK_H
#define CONTROLLERFREETOSTASK_H
#include <FreeTOSTask.h>
#include <uart/Uart.h>
#include "DataSampleTimer.h"
#include "MotorDriver.h"
#include "TransposedIIRFilter.h"



class ControllerFreeTOSTask final : public FreeTOSTask {
public:
    ControllerFreeTOSTask(Uart& uart, DataSampleTimer & data_sample_timer, const MotorDriver & motor1_driver, const MotorDriver & motor2_driver);

    void run() override;

    const Uart&      m_uart;
    DataSampleTimer& m_data_sample_timer;

    /* Computed in task context after each FreeRTOS wake-up — not shared with
     * any ISR, so volatile is not needed here. */
    float  m_left_wheel_distance  {0.0F};
    float  m_right_wheel_distance {0.0F};
    double m_left_read_w  {0.0};
    double m_right_read_w {0.0};
    double wLeft  {0.0};
    double wRight {0.0};
    double m_ref_w {0.0};
    int minimumOutput;
    int maximumOutput;
    TransposedIIRFilter m_delta_us_filter_left;
    TransposedIIRFilter m_delta_us_filter_right;
    TransposedIIRFilter m_pi_control_filter_left;
    TransposedIIRFilter m_pi_control_filter_right;
    const MotorDriver & m_motor1_driver;
    const MotorDriver & m_motor2_driver;
};



#endif //CONTROLLERFREETOSTASK_H
