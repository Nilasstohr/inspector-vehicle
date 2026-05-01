//
// Created by robotcentral on 4/30/26.
//

#ifndef SAMPLETIMER_H
#define SAMPLETIMER_H

#include <cstdint>
#include "FreeRTOS.h"
#include "task.h"
#include "Encoder.h"

/**
 * @class DataSampleTimer
 * @brief Receives 100 µs ticks from TIM2 and snapshots raw encoder state.
 *
 * Mirrors the Encoder pattern:
 *   - registerInstance() wires the object to the static ISR trampoline.
 *   - isr() is called directly from TIM2_IRQHandler in isr_handlers.cpp.
 *   - All ISR work stays in handleTick() — no blocking FreeRTOS calls.
 *
 * Lock-free design
 * ────────────────
 * handleTick() reads only m_count (int32_t) and m_tickDeltaUs (uint32_t) from
 * each encoder — both 32-bit aligned, so each read is a single LDR instruction
 * (atomically safe on Cortex-M7).  The results are stored as the same 32-bit
 * types (single STR each).  No float/double maths are performed in ISR context.
 *
 * ControllerFreeRTOSTask wakes via FreeRTOS task-notification (which acts as a
 * full memory barrier) and computes wheelDistance / angularVelocity from the
 * snapshots entirely in task context — no critical sections needed anywhere.
 */
class DataSampleTimer {
public:
    DataSampleTimer(const Encoder & motor1_encoder, const Encoder & motor2_encoder);

    DataSampleTimer(const DataSampleTimer&)            = delete;
    DataSampleTimer& operator=(const DataSampleTimer&) = delete;
    DataSampleTimer(DataSampleTimer&&)                 = delete;
    DataSampleTimer& operator=(DataSampleTimer&&)      = delete;

    /** Wire this instance to the ISR trampoline — call before start(). */
    static void registerInstance(DataSampleTimer& inst);

    /** Called by TIM2_IRQHandler — do not call from application code. */
    static void isr();

    /** @return Total tick count (each tick = 100 µs). */
    [[nodiscard]] uint32_t tickCount() const;

    /**
     * @brief Register the FreeRTOS task to be notified after each ISR tick.
     *
     * Call once from the task's run() body before entering the event loop:
     * @code
     *   m_data_sample_timer.setNotifyTask(xTaskGetCurrentTaskHandle());
     * @endcode
     * The ISR will call vTaskNotifyGiveFromISR() at the end of every handleTick(),
     * replacing the vTaskDelay polling loop with an event-driven wake-up.
     */
    void setNotifyTask(TaskHandle_t handle);

    /**
     * @brief Raw encoder snapshots — written by ISR as single 32-bit stores,
     *        read by ControllerFreeRTOSTask as single 32-bit loads.
     *        No critical section required on Cortex-M7.
     */
    [[nodiscard]] int32_t  getLeftCount()        const { return m_left_count; }
    [[nodiscard]] uint32_t getLeftTickDeltaUs()  const { return m_left_tick_delta_us; }
    [[nodiscard]] int32_t  getRightCount()       const { return m_right_count; }
    [[nodiscard]] uint32_t getRightTickDeltaUs() const { return m_right_tick_delta_us; }

private:
    const Encoder& m_encoder1;
    const Encoder& m_encoder2;

    /* Raw 32-bit snapshots — each written/read with a single STR/LDR (atomic). */
    volatile int32_t  m_left_count          {0};
    volatile uint32_t m_left_tick_delta_us  {0U};
    volatile int32_t  m_right_count         {0};
    volatile uint32_t m_right_tick_delta_us {0U};

    void         handleTick();   /* actual per-tick logic — runs in ISR context */
    TaskHandle_t m_notify_task = nullptr; /* task woken after each tick */
    static DataSampleTimer* s_instance;
};

#endif //SAMPLETIMER_H
