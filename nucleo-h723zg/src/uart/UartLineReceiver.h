/**
 * @file    UartLineReceiver.h
 * @brief   Interrupt-driven UART line receiver.
 *
 * Architecture:
 *   HAL_UART_Receive_IT() is armed for 1 byte at a time.
 *   Each byte fires HAL_UART_RxCpltCallback → UartLineReceiver::onByteReceived().
 *   The ISR accumulates bytes until '\n' is found, then:
 *     1. Snapshots the complete line into m_completeLine[].
 *     2. Notifies the registered task via vTaskNotifyGiveFromISR().
 *   The task calls waitLine() which blocks on ulTaskNotifyTake() and returns
 *   a string_view of the complete line once signalled.
 *
 * Thread safety:
 *   m_completeLine is only written in the ISR and only read by the task AFTER
 *   the notification is received — no concurrent access is possible.
 *   NVIC priority must be in the managed zone (≥ configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)
 *   so that vTaskNotifyGiveFromISR() is safe.
 */
#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "stm32h7xx_hal.h"
#include <cstddef>
#include <string_view>

static constexpr size_t kUartLineBufferSize = 128U;  /* max line length incl. '\n' */

class UartLineReceiver {
public:
    explicit UartLineReceiver(UART_HandleTypeDef& huart);

    UartLineReceiver(const UartLineReceiver&)            = delete;
    UartLineReceiver& operator=(const UartLineReceiver&) = delete;
    UartLineReceiver(UartLineReceiver&&)                 = delete;
    UartLineReceiver& operator=(UartLineReceiver&&)      = delete;

    /**
     * Register the calling task as the notification target and arm the first
     * HAL_UART_Receive_IT.  Call once at the top of the task's run() loop,
     * BEFORE entering the for(;;) so the handle is set before any bytes arrive.
     */
    void start();

    /**
     * Block the calling task until a complete '\n'-terminated line is received.
     * Returns a string_view into the internal buffer — valid until the next call.
     * Must only be called from the one registered task.
     */
    [[nodiscard]] std::string_view waitLine();

    /**
     * Called from USART3_IRQHandler (ISR context) with the byte already read
     * from RDR.  Do NOT call from task context.
     */
    void handleRxByte(uint8_t byte);

    /** Singleton accessor used by the ISR in isr_handlers.cpp. */
    [[nodiscard]] static UartLineReceiver* instance();
    static void setInstance(UartLineReceiver* inst);

private:
    UART_HandleTypeDef& m_huart;
    TaskHandle_t        m_taskHandle{nullptr};

    char    m_lineBuf[kUartLineBufferSize]{};        /* accumulation buffer (ISR)   */
    size_t  m_linePos{0U};                           /* write index into m_lineBuf  */

    char    m_completeLine[kUartLineBufferSize]{};   /* snapshot on '\n' (ISR→task) */
    size_t  m_completeLen{0U};                       /* length of m_completeLine    */

    static UartLineReceiver* s_instance;
};

