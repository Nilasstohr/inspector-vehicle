#pragma once

#include <gpio/GpioOutput.h>

#include "FreeRTOS.h"
#include "task.h"

/**
 * @class BlinkyFreeRTOSTask
 * @brief Encapsulates a FreeRTOS task that blinks a GPIO pin.
 *
 * Accepts a GpioOutput reference so pin ownership and init remain with the
 * caller (main). The task only calls toggle() on the provided output.
 *
 * C++ pattern for FreeRTOS tasks:
 *   - Constructor stores the GpioOutput reference and blink interval
 *   - start() creates the FreeRTOS task
 *   - Static taskEntry() bridges C FreeRTOS API → C++ run() method
 */
class BlinkyFreeRTOSTask {
   public:
    /**
     * @param pin         GPIO output to blink (caller owns the object)
     * @param intervalMs  Toggle period in milliseconds
     */
    BlinkyFreeRTOSTask(GpioOutput& pin, uint32_t intervalMs);

    /* Once 'this' is passed to xTaskCreate, the object address is captured.
     * Copying or moving would leave the running task with a dangling pointer. */
    BlinkyFreeRTOSTask(const BlinkyFreeRTOSTask&)            = delete;
    BlinkyFreeRTOSTask& operator=(const BlinkyFreeRTOSTask&) = delete;
    BlinkyFreeRTOSTask(BlinkyFreeRTOSTask&&)                 = delete;
    BlinkyFreeRTOSTask& operator=(BlinkyFreeRTOSTask&&)      = delete;

    /* Create the FreeRTOS task — call once before vTaskStartScheduler() */
    void start(const char* name = "Blinky", UBaseType_t priority = 1, uint16_t stackWords = 256);

    /* Change blink rate at runtime (safe to call from another task) */
    void setInterval(uint32_t intervalMs);

    /* Read current blink interval (used by tests) */
    [[nodiscard]] uint32_t getInterval() const { return m_intervalMs; }

   private:
    /* Static trampoline: FreeRTOS needs a plain C function pointer */
    static void taskEntry(void* arg);

    /* The actual task loop — runs as a member function */
    void run();

    GpioOutput&  m_pin;
    uint32_t     m_intervalMs;
    TaskHandle_t m_handle = nullptr;
};
