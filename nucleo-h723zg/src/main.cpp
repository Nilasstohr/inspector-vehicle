
/**
 * @file    main.cpp
 * @brief   FreeRTOS blinky for Nucleo-H723ZG (C++)
 */

#include <host_command_handler/HostCommandHandlerFreeTOSTask.h>

#include "BlinkyFreeRTOSTask.h"
#include "CrashHandler.h"
#include "FreeRTOS.h"
#include "stm32h7xx_hal.h"
#include "task.h"

/* ── LED pin definitions (Nucleo-H723ZG MB1364) ─────────────────────────── */
#define LD1_PIN GPIO_PIN_0  /* Green  - PB0  */
#define LD2_PIN GPIO_PIN_1  /* Yellow - PE1  */
#define LD3_PIN GPIO_PIN_14 /* Red    - PB14 */

/* ── Function prototypes ────────────────────────────────────────────────── */
static void SystemClock_Config(void);
static void GPIO_Init(void);
static void UART3_Init(void);

/* ── UART handle ─────────────────────────────────────────────────────────── */
/* USART3 (PD8/PD9) is routed to the ST-Link VCP on Nucleo-H723ZG.         */
/* Connect a serial terminal on the PC at 115200 8N1 to see log output.    */
UART_HandleTypeDef huart3;

/* ── Logger task ─────────────────────────────────────────────────────────── */
/* Runs at the lowest user priority (1). Prints a counter every 2 seconds.  */

/* ── main ───────────────────────────────────────────────────────────────── */
int main(void) {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    UART3_Init();

    static UartTransceiver uartTransmitter = UartTransceiver(huart3);
    static HostCommandHandlerFreeTOSTask hostHandler(uartTransmitter, 10);
    //static LoggerFreeRTOSTask loggerTask(uartTransmitter);

    /* Report any fault saved from the previous run before starting tasks */
    CrashHandler_checkAndReport(&hostHandler.getUart());
     /* Should be caught by HardFault_HandlerC and logged on next boot */
    //configASSERT(false);  /* Intentional assert → UDF #0 → HardFault (test CrashHandler) */
    //*reinterpret_cast<volatile uint32_t*>(0x00000000u) = 0xDEAD;  /* NULL write → HardFault */

    static BlinkyFreeRTOSTask greenLed(GPIOB, LD1_PIN, 1000);   /* 1Hz   */

    greenLed.start("GreenLED", 1);
    hostHandler.start(1, 512);  /* 512 words — extra headroom for float printf */

    vTaskStartScheduler();
    while (1);
}

/* ── System clock: keep HSI 64MHz default, no supply change ─────────────── */
static void SystemClock_Config(void) {
    /* On reset, STM32H7 already runs on HSI at 64MHz (default).
     * We do NOT touch power supply config here — the Nucleo-H723ZG uses
     * direct SMPS internally and calling PWR_LDO_SUPPLY would hang. */
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 |
                                  RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}

/* ── GPIO: init all 3 user LEDs ─────────────────────────────────────────── */
static void GPIO_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    /* PB0 = LD1 green, PB14 = LD3 red */
    GPIO_InitStruct.Pin = LD1_PIN | LD3_PIN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* PE1 = LD2 yellow */
    GPIO_InitStruct.Pin = LD2_PIN;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/* ── UART3 init: ST-Link VCP on PD8 (TX) / PD9 (RX) @ 115200 8N1 ──────── */
static void UART3_Init(void) {
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &gpio);

    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    huart3.FifoMode = UART_FIFOMODE_ENABLE;
    HAL_UART_Init(&huart3);
}

/* ── All ISRs and FreeRTOS hooks need extern "C" in a .cpp file ─────────── *
 * Without it C++ name-mangling renames them and the vector table falls      *
 * back to Default_Handler (infinite loop).                                  */
extern "C" {

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    __disable_irq();
    while (1);
}

void vApplicationMallocFailedHook(void) {
    __disable_irq();
    while (1);
}
extern void xPortSysTickHandler(void);

void SysTick_Handler(void) {
    HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState == 1)
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
#endif
}

/**
 * ASM trampoline: determines which stack was active (MSP or PSP)
 * and passes the correct frame pointer to HardFault_HandlerC (CrashHandler.cpp).
 */
__attribute__((naked))
void HardFault_Handler(void)
{
    __asm volatile (
        " tst   lr, #4             \n"  /* Test EXC_RETURN bit 2 */
        " ite   eq                 \n"
        " mrseq r0, msp            \n"  /* CPU was on MSP (main/ISR stack) */
        " mrsne r0, psp            \n"  /* CPU was on PSP (task stack)     */
        " b     HardFault_HandlerC \n"
    );
}

}  // extern "C"
