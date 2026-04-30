//
// Created by robotcentral on 4/23/26.

#include "HostCommandHandlerFreeTOSTask.h"
#include <uart/UartTransceiver.h>
#include "ReceiveCommand.h"
#include "VelocityCommand.h"

/**
 *
 * @param uart
 * @param transceiver
 * @param intervalMs
 * @param encoder
 */
HostCommandHandlerFreeTOSTask::HostCommandHandlerFreeTOSTask(const Uart & uart):
FreeTOSTask(),m_uart(uart) {}


/* ── run() — the actual task loop ──────────────────────────────────────── */
void HostCommandHandlerFreeTOSTask::run() const {
    char buf[128];
    for (;;) {
        auto result = m_uart.getTransceiver().receive(buf, sizeof(buf));
        if (result.has_value() && result.value() > 0) {
            auto received = std::string_view(buf, result.value());
            if (ReceiveCommand cmd(received); cmd.valid()) {
                if (cmd.command() == HostCommandName::Vel) {
                    if (VelocityCommand velocity_command(cmd.args()); velocity_command.valid()) {
                        m_uart.getTransceiver().transmit("ack", 3);
                    } else {
                        m_uart.logf("Invalid velocity args: %s\n", received.data());
                    }
                } else {
                    m_uart.logf("Unrecognized command\n");
                }
            } else {
                m_uart.logf("Invalid command: %s\n", received.data());
            }
        }
        // no vTaskDelay — receive() blocks waiting for data
    }
}
