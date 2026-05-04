//
// Created by robotcentral on 4/30/26.
//

#include "DataSampleTimer.h"

/* ── Static registry ──────────────────────────────────────────────────────── */
DataSampleTimer* DataSampleTimer::s_instance = nullptr;

DataSampleTimer::DataSampleTimer(const Encoder &motor1_encoder, const Encoder &motor2_encoder):
    m_encoder1(motor1_encoder), m_encoder2(motor2_encoder) {}

/* ── Registration ─────────────────────────────────────────────────────────── */
void DataSampleTimer::registerInstance(DataSampleTimer& inst)
{
    s_instance = &inst;
}

/* ── ISR trampoline ───────────────────────────────────────────────────────── */
void DataSampleTimer::isr()
{
    if (s_instance != nullptr) {
        s_instance->handleTick();
    }
}

/* ── Registration of notify task ──────────────────────────────────────────── */
void DataSampleTimer::setNotifyTask(TaskHandle_t handle)
{
    m_notify_task = handle;
}


/* ── Per-tick logic (ISR context — no blocking calls, no float/double maths) ─ */
void DataSampleTimer::handleTick()
{
    /* Snapshot raw encoder state — each is a single 32-bit LDR (atomic on
     * Cortex-M7).  No critical section needed.  The heavy maths (wheelDistance,
     * angularVelocity) are deferred to ControllerFreeRTOSTask in task context. */
    m_left_count          = m_encoder1.getCount();
    m_left_tick_delta_us  = m_encoder1.getTickDeltaUs();
    m_right_count         = m_encoder2.getCount();
    m_right_tick_delta_us = m_encoder2.getTickDeltaUs();

    /* Wake the controller task, if registered. */
    if (m_notify_task != nullptr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(m_notify_task, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

