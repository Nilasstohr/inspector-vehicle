#pragma once

#include <freetos/FreeTOSTask.h>
#include <gpio/GpioOutput.h>
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
class BlinkyFreeRTOSTask: public FreeTOSTask {
public:
    /**
     * @param pin         GPIO output to blink (caller owns the object)
     * @param intervalMs  Toggle period in milliseconds
     */
    BlinkyFreeRTOSTask(GpioOutput& pin, uint32_t intervalMs);

private:/* The actual task loop — runs as a member function */
    void run() override;
    GpioOutput&  m_pin;
    uint32_t m_intervalMs;
};
