#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* ── Scheduler ──────────────────────────────────────────────────────────── */
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_TICKLESS_IDLE                 0

/* ── Clock ──────────────────────────────────────────────────────────────── */
/* HSI = 64MHz, no PLL for simplicity */
#define configCPU_CLOCK_HZ                      64000000UL
#define configTICK_RATE_HZ                      1000  /* 1ms tick */

/* ── Tasks ──────────────────────────────────────────────────────────────── */
#define configMAX_PRIORITIES                    8
#define configMINIMAL_STACK_SIZE                256  /* in words */
#define configMAX_TASK_NAME_LEN                 16
#define configIDLE_SHOULD_YIELD                 1

/* ── Heap (heap_4 allocator, array placed in AXI SRAM via linker .bss) ──── */
#define configTOTAL_HEAP_SIZE                   (32 * 1024)  /* 32KB */
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         0

/* ── Features ───────────────────────────────────────────────────────────── */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_QUEUE_SETS                    0
#define configQUEUE_REGISTRY_SIZE               8
#define configUSE_16_BIT_TICKS                  0

/* ── Timers ─────────────────────────────────────────────────────────────── */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                5
#define configTIMER_TASK_STACK_DEPTH            512

/* ── Debug hooks ────────────────────────────────────────────────────────── */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          2  /* catches stack overflows */
#define configUSE_MALLOC_FAILED_HOOK            1  /* catches heap exhaustion */

/* ── Cortex-M7 interrupt priority config ────────────────────────────────── */
/*
 * UNDERSTANDING INTERRUPT PRIORITIES WITH FreeRTOS ON STM32H7
 * ─────────────────────────────────────────────────────────────
 *
 * 1. THE HARDWARE PRIORITY SCALE
 *    The STM32H7 Cortex-M7 core has 4 priority bits, giving 16 levels:
 *      0  = most urgent  (fires first, can interrupt everything below it)
 *      15 = least urgent (fires last, can be interrupted by everything above it)
 *
 *    You pass this number to HAL_NVIC_SetPriority().
 *
 * 2. WHAT IS "THE FreeRTOS API"?
 *    FreeRTOS provides functions your code can call, e.g.:
 *      vTaskDelay()               — sleep for N ticks
 *      xQueueSend()               — put data in a queue
 *      xTaskNotifyFromISR()       — wake a sleeping task from inside an ISR
 *      xSemaphoreGiveFromISR()    — release a semaphore from inside an ISR
 *
 *    The "FromISR" suffix marks functions that are safe to call from an
 *    interrupt handler.  Non-FromISR functions (e.g. xQueueSend) must NEVER
 *    be called from an ISR — they will corrupt the kernel.
 *
 * 3. THE PROBLEM: FreeRTOS USES INTERRUPTS INTERNALLY
 *    FreeRTOS itself runs on interrupts:
 *      SysTick_Handler  — fires every 1 ms to advance the tick counter and
 *                         potentially switch which task is running.
 *      PendSV_Handler   — does the actual context switch (saves/restores
 *                         task registers).
 *      SVC_Handler      — used to start the scheduler for the first time.
 *
 *    These kernel ISRs are set to the LOWEST priority (15) so that your
 *    application ISRs can interrupt them.
 *
 *    When a "FromISR" function decides a higher-priority task should run,
 *    it does NOT switch immediately — it just sets the PendSV pending flag
 *    and returns.  The actual switch happens when PendSV fires.
 *
 *    For this mechanism to work safely, FreeRTOS must be able to briefly
 *    "mask" (temporarily disable) a set of interrupts while it updates its
 *    internal data structures.  It does this with the BASEPRI register on
 *    Cortex-M, which masks all interrupts AT OR BELOW a given priority level.
 *
 * 4. THE TWO ZONES  (the critical concept)
 *
 *    configMAX_SYSCALL_INTERRUPT_PRIORITY = 5  (the boundary)
 *
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │  Priority │  Zone   │  What happens                                 │
 *    │───────────│─────────│───────────────────────────────────────────────│
 *    │  0        │         │  e.g. a safety emergency stop — fires always  │
 *    │  1        │  ABOVE  │                                               │
 *    │  2        │  the    │  FreeRTOS cannot mask these.  They can fire   │
 *    │  3        │  limit  │  even while FreeRTOS is updating its internal │
 *    │  4        │  ⚡     │  lists.  Calling ANY FreeRTOS function here   │
 *    │           │         │  risks reading half-updated data → crash.     │
 *    │───────────│─────────│───────────────────────────────────────────────│
 *    │  5   ◄────┼── limit │  Highest priority where FromISR calls are     │
 *    │  6        │  BELOW  │  safe.  FreeRTOS masks priorities 5–15 during │
 *    │  7        │  the    │  critical sections, so its data is always     │
 *    │  8        │  limit  │  consistent when your ISR runs.               │
 *    │  ...      │  ✅     │                                               │
 *    │  14       │         │  PendSV / SysTick live here (priority 15)     │
 *    │  15       │         │                                               │
 *    └─────────────────────────────────────────────────────────────────────┘
 *
 *    "Managed"  = FreeRTOS knows about these IRQs, can mask them, FromISR safe.
 *    "Unmanaged"= FreeRTOS is completely blind to these IRQs — they are yours
 *                 alone; use them only for bare-metal, zero-OS work.
 *
 * 5. PRACTICAL RULE FOR THIS PROJECT
 *    Any ISR that calls a FreeRTOS FromISR function (or might do so in the
 *    future) MUST be at priority 5 or higher NUMBER (5, 6, 7 … 15).
 *    Priority 5 is the sweet spot: maximum urgency while still being managed.
 *
 *    Our encoder ISR currently does not call FreeRTOS at all, but it is set
 *    to 5 so that adding xTaskNotifyFromISR() later needs zero config changes.
 *
 * 6. FURTHER READING
 *    https://www.freertos.org/RTOS-Cortex-M3-M4.html
 *    (The page is for M3/M4 but the BASEPRI mechanism is identical on M7.)
 *    Search for "configMAX_SYSCALL_INTERRUPT_PRIORITY" on that page.
 */
#define configPRIO_BITS                         4   /* STM32H7 uses 4 priority bits → 16 levels (0–15) */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15  /* least urgent; used for PendSV / SysTick */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5   /* boundary: priorities 5–15 are FreeRTOS-managed */
#define configKERNEL_INTERRUPT_PRIORITY   (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* ── INCLUDE functions ──────────────────────────────────────────────────── */
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xResumeFromISR                  1

/* ── Assert ─────────────────────────────────────────────────────────────────
 * configASSERT triggers a HardFault in both debug and release builds.
 * The undefined instruction (UDF #0) raises a UsageFault which escalates to
 * HardFault, so CrashHandler will capture and persist the fault frame.
 * ──────────────────────────────────────────────────────────────────────────── */
#define configASSERT(x) \
    do { if (!(x)) { __asm volatile ("udf #0"); } } while (0)


#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler

#endif /* FREERTOS_CONFIG_H */



