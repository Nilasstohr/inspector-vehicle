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
#define configPRIO_BITS                         4   /* STM32H7 uses 4 priority bits */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
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



