//
// Created by robotcentral on 4/23/26.

#include "HostCommandHandlerFreeTOSTask.h"

#include "DistanceReadRequestCommand.h"
#include "ReceiveCommand.h"
#include "VelocityCommand.h"

HostCommandHandlerFreeTOSTask::HostCommandHandlerFreeTOSTask(
    const Uart&       uart,
    UartLineReceiver& lineReceiver,
    PiMotorControl&   pi_motor_control)
    : FreeTOSTask(),
      m_uart(uart),
      m_lineReceiver(lineReceiver),
      m_pi_motor_control(pi_motor_control) {}


/* ── run() — the actual task loop ──────────────────────────────────────── */
void HostCommandHandlerFreeTOSTask::run() {
    /* Register this task as the notification target and arm the first IT receive.
     * Must be called inside run() so that xTaskGetCurrentTaskHandle() is valid. */
    m_lineReceiver.start();

    for (;;) {
        /* Block here until the ISR signals that a complete '\n'-terminated
         * line has been accumulated.  No polling, no busy-wait. */
        const std::string_view line = m_lineReceiver.waitLine();

        if (line.empty()) {
            continue;
        }

        if (ReceiveCommand cmd(line); cmd.valid()) {
            switch (cmd.command()) {
                case HostCommandName::Vel: {
                    if (VelocityCommand velocity_command(cmd.args()); velocity_command.valid()) {
                        m_uart.getTransceiver().transmit("ack", 3);
                        m_uart.logf(" - l:%.2f  r:%.2f\n\r",
                                    velocity_command.left(),
                                    velocity_command.right());
                        m_pi_motor_control.setVelocities(velocity_command.left(),
                                                         velocity_command.right());
                    } else {
                        m_uart.logf("Invalid velocity args: %.*s\n",
                                    static_cast<int>(line.size()), line.data());
                    }
                    break;
                }
                case HostCommandName::Dis: {
                    const float left_distance  = m_pi_motor_control.getLeftWheelDistance();
                    const float right_distance = m_pi_motor_control.getRightWheelDistance();
                    const auto  response       = DistanceReadRequestCommand::handle(left_distance,
                                                                              right_distance);
                    m_uart.getTransceiver().transmit(response.c_str(),
                                                     static_cast<uint16_t>(response.size()));
                    m_uart.getTransceiver().transmit("ack", 3);
                    break;
                }
                default:
                    m_uart.logf("Unrecognized command\n");
            }
        } else {
            m_uart.logf("Invalid command: %.*s\n",
                        static_cast<int>(line.size()), line.data());
        }
    }
}


