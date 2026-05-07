#include "UartLineReceiver.h"
#include <cstring>

/* ── Singleton storage ──────────────────────────────────────────────────── */
UartLineReceiver* UartLineReceiver::s_instance = nullptr;

UartLineReceiver* UartLineReceiver::instance() {
    return s_instance;
}

void UartLineReceiver::setInstance(UartLineReceiver* inst) {
    s_instance = inst;
}

/* ── Constructor ────────────────────────────────────────────────────────── */
UartLineReceiver::UartLineReceiver(UART_HandleTypeDef& huart)
    : m_huart(huart) {}

/* ── start() — call once from the task's run(), before the for(;;) ──────── */
void UartLineReceiver::start() {
    /* Register the task handle so the ISR can notify it.
     * RXNEIE is enabled statically in UART3_Init — no HAL_UART_Receive_IT needed. */
    m_taskHandle = xTaskGetCurrentTaskHandle();
}

/* ── handleRxByte() — called from USART3_IRQHandler with the byte from RDR ─ */
void UartLineReceiver::handleRxByte(const uint8_t byte) {
    const char ch = static_cast<char>(byte);

    /* Silently discard bare CR — host typically sends CR+LF or LF only */
    if (ch == '\r') {
        return;
    }

    if (m_linePos < (kUartLineBufferSize - 1U)) {
        m_lineBuf[m_linePos] = ch;
        m_linePos++;
    }
    /* else: line too long — keep discarding until '\n' to re-sync */

    if (ch == '\n') {
        /* Null-terminate and snapshot the complete line */
        m_lineBuf[m_linePos] = '\0';
        m_completeLen = m_linePos;
        memcpy(m_completeLine, m_lineBuf, m_completeLen + 1U);
        m_linePos = 0U;  /* reset accumulator for next line */

        /* Signal the waiting task — ISR-safe, priority must be ≥ configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
        if (m_taskHandle != nullptr) {
            BaseType_t higherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(m_taskHandle, &higherPriorityTaskWoken);
            portYIELD_FROM_ISR(higherPriorityTaskWoken);
        }
    }
}

/* ── waitLine() — blocks the task until a complete line is ready ─────────── */
std::string_view UartLineReceiver::waitLine() {
    /* Block indefinitely; unblocked by vTaskNotifyGiveFromISR in handleRxByte() */
    ulTaskNotifyTake(pdTRUE, /*timeout=*/portMAX_DELAY);
    return {m_completeLine, m_completeLen};
}
