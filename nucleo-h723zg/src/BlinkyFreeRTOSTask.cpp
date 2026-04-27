#include "BlinkyFreeRTOSTask.h"

/* ── Constructor ────────────────────────────────────────────────────────── */
BlinkyFreeRTOSTask::BlinkyFreeRTOSTask(GPIO_TypeDef* port,
                                       uint16_t      pin,
                                       uint32_t      intervalMs)
    : m_port(port), m_pin(pin), m_intervalMs(intervalMs)
{
}

/* ── start() ────────────────────────────────────────────────────────────── */
void BlinkyFreeRTOSTask::start(const char* name, UBaseType_t priority,
                               uint16_t stackWords)
{
    /* Pass 'this' as the argument so taskEntry() can call run() */
    xTaskCreate(taskEntry, name, stackWords, this, priority, &m_handle);
}

/* ── setInterval() ──────────────────────────────────────────────────────── */
void BlinkyFreeRTOSTask::setInterval(uint32_t intervalMs)
{
    m_intervalMs = intervalMs;  /* atomic on 32-bit ARM */
}

/* ── taskEntry() — static trampoline ────────────────────────────────────── */
void BlinkyFreeRTOSTask::taskEntry(void* arg)
{
    /* Recover the object pointer and call the member function */
    static_cast<BlinkyFreeRTOSTask*>(arg)->run();
}

/* ── run() — the actual task loop ──────────────────────────────────────── */
void BlinkyFreeRTOSTask::run() const {
    for (;;)
    {
        HAL_GPIO_TogglePin(m_port, m_pin);
        vTaskDelay(pdMS_TO_TICKS(m_intervalMs));
    }
}

