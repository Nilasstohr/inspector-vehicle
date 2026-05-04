#include "BlinkyFreeRTOSTask.h"

/* ── Constructor ────────────────────────────────────────────────────────── */
BlinkyFreeRTOSTask::BlinkyFreeRTOSTask(GpioOutput& pin, uint32_t intervalMs)
    :FreeTOSTask(), m_pin(pin),m_intervalMs(intervalMs)
{
}

/* ── run() — the actual task loop ──────────────────────────────────────── */
void BlinkyFreeRTOSTask::run() {
    for (;;)
    {
        m_pin.toggle();
        vTaskDelay(pdMS_TO_TICKS(m_intervalMs));
    }
}
