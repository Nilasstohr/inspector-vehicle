//
// Created by robotcentral on 4/23/26.

#include "HostCommandHandlerFreeTOSTask.h"
#include <uart/UartTransceiver.h>

#include "DistanceReadRequestCommand.h"
#include "ReceiveCommand.h"
#include "VelocityCommand.h"

/**
 *
 * @param uart
 * @param line_receiver
 * @param pi_motor_control
 * @param transceiver
 * @param intervalMs
 * @param encoder
 */
HostCommandHandlerFreeTOSTask::HostCommandHandlerFreeTOSTask(
    const Uart & uart, UartLineReceiver & line_receiver, PiMotorControl &pi_motor_control):
FreeTOSTask(),m_uart(uart),m_line_receiver(line_receiver),m_pi_motor_control(pi_motor_control) {}


/* ── run() — the actual task loop ──────────────────────────────────────── */
void HostCommandHandlerFreeTOSTask::run(){
    char buf[128];
    /* Register this task as the notification target and arm the first IT receive.
     * Must be called inside run() so that xTaskGetCurrentTaskHandle() is valid. */
    m_line_receiver.start();
    for (;;) {
        /* Block here until the ISR signals that a complete '\n'-terminated
         * line has been accumulated.  No polling, no busy-wait. */
        const std::string_view line = m_line_receiver.waitLine();

        if (line.empty()) {
            continue;
        }

        if (ReceiveCommand cmd(line); cmd.valid()) {
            handleCommand(cmd, line);
        } else {
            m_uart.logf("Invalid command: %.*s\n", static_cast<int>(line.size()), line.data());
        }
    }
}

void HostCommandHandlerFreeTOSTask::handleCommand(const ReceiveCommand & cmd, const std::string_view  & line) const {
    switch (cmd.command()) {
        case HostCommandName::Vel: {
            if (VelocityCommand velocity_command(cmd.args()); velocity_command.valid()) {
                //m_uart.logf(" - l:%.2f  r:%.2f\n\r",velocity_command.left(),velocity_command.right());
                m_pi_motor_control.setVelocities(velocity_command.left(),velocity_command.right());
                m_uart.getTransceiver().transmit("ack\n", 4);
            } else {
                m_uart.logf("Invalid velocity args: %.*s\n",static_cast<int>(line.size()), line.data());
            }
            break;
        }
        case HostCommandName::Dis: {
            const float left_distance  = m_pi_motor_control.getLeftWheelDistance();
            const float right_distance = m_pi_motor_control.getRightWheelDistance();
            const auto  response       = DistanceReadRequestCommand::handle(left_distance,right_distance);
            m_uart.getTransceiver().transmit(response.c_str(),static_cast<uint16_t>(response.size()));
            m_uart.getTransceiver().transmit("ack\n", 4);
            break;
        }
        case HostCommandName::Reset: {
            m_pi_motor_control.reset();
            m_uart.getTransceiver().transmit("ack\n", 4);
            break;
        }
        case HostCommandName::Stop: {
            m_pi_motor_control.setVelocities(0.0f, 0.0f);
            m_uart.getTransceiver().transmit("ack\n", 4);
            break;
        }
        default:
            m_uart.logf("Unrecognized command\n");
    }
}
