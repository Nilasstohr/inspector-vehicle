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
 * @param pi_motor_control
 * @param transceiver
 * @param intervalMs
 * @param encoder
 */
HostCommandHandlerFreeTOSTask::HostCommandHandlerFreeTOSTask(
    const Uart & uart, PiMotorControl &pi_motor_control):
FreeTOSTask(),m_uart(uart),m_pi_motor_control(pi_motor_control) {}


/* ── run() — the actual task loop ──────────────────────────────────────── */
void HostCommandHandlerFreeTOSTask::run(){
    char buf[128];
    for (;;) {
        auto result = m_uart.getTransceiver().receive(buf, sizeof(buf));
        if (result.has_value() && result.value() > 0) {
            auto received = std::string_view(buf, result.value());
            if (ReceiveCommand cmd(received); cmd.valid()) {
                handleCommand(cmd, received);
            } else {
                m_uart.logf("Invalid command: %s\n", received.data());
            }
        }
        // no vTaskDelay — receive() blocks waiting for data
    }
}

void HostCommandHandlerFreeTOSTask::handleCommand(const ReceiveCommand & cmd, const std::string_view  & received) const {
    switch (cmd.command()) {
        case HostCommandName::Vel: {
            if (VelocityCommand velocity_command(cmd.args()); velocity_command.valid()) {
                m_uart.getTransceiver().transmit("ack", 3);
                m_uart.logf(" - l:%.2f  r:%.2f\n\r",velocity_command.left(),velocity_command.right());
                m_pi_motor_control.setVelocities(velocity_command.left(),velocity_command.right());
            } else {
                m_uart.logf("Invalid velocity args: %s\n", received.data());
            }
            break;
        }
        case HostCommandName::Dis: {
            const float left_distance = m_pi_motor_control.getLeftWheelDistance();
            const float right_distance = m_pi_motor_control.getRightWheelDistance();
            auto response = DistanceReadRequestCommand::handle(left_distance, right_distance);
            m_uart.getTransceiver().transmit(response.c_str(), response.size());
            m_uart.getTransceiver().transmit("ack", 3);
            break;
        }
        case HostCommandName::Reset: {
            m_uart.getTransceiver().transmit("ack", 3);
            m_pi_motor_control.reset();
            break;
        }
        default:
            m_uart.logf("Unrecognized command\n");
    }
}
