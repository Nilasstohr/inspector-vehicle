/**
 * @file    main.cpp
 * @brief   FreeRTOS blinky for Nucleo-H723ZG (C++)
 */

#include <gpio/GpioOutput.h>
#include <host_command_handler/HostCommandHandlerFreeTOSTask.h>
#include <motors/MotorDriver.h>
#include <pwm/PwmOutput.h>

#include "BlinkyFreeRTOSTask.h"
#include "CrashHandler.h"
#include "FreeRTOS.h"
#include "stm32h7xx_hal.h"
#include "task.h"

/* ── Function prototypes ────────────────────────────────────────────────── */
static void SystemClock_Config(void);
static void UART3_Init(void);

/* ── UART handle ─────────────────────────────────────────────────────────── */
/* USART3 (PD8/PD9) is routed to the ST-Link VCP on Nucleo-H723ZG.         */
/* Connect a serial terminal on the PC at 115200 8N1 to see log output.    */
UART_HandleTypeDef huart3;


/* ── main ───────────────────────────────────────────────────────────────── */
[[noreturn]] int main(void) {
    HAL_Init();
    SystemClock_Config();
    UART3_Init();

    static UartTransceiver uartTransmitter = UartTransceiver(huart3);
    static HostCommandHandlerFreeTOSTask hostHandler(uartTransmitter, 10);

    /* Report any fault saved from the previous run before starting tasks */
    CrashHandler_checkAndReport(&hostHandler.getUart());

    /* ── PG12 (CN10 pin 43) — general-purpose output ──────────────────────── */
    /* Source: UM2407 STM32H7 Nucleo-144 MB1364, Table 20, CN10 connector     */
    // Motor 1
    static PwmOutput motor1PWM (TIM1, TIM_CHANNEL_1);  /* PE9  —  D6 (CN10) */
    static GpioOutput motor1INA(GPIOG, GPIO_PIN_12);  /*  D7 starts LOW */
    static GpioOutput motor1INB(GPIOE, GPIO_PIN_14);  /*  D4 starts LOW */
    static MotorDriver motor1(motor1PWM, motor1INA, motor1INB);
    // Motor 2
    static PwmOutput motor2PWM(TIM1, TIM_CHANNEL_2);  /* PE11 —  D5 (CN10) */
    static GpioOutput motor2INA(GPIOE, GPIO_PIN_13);  /* D3  starts LOW */
    static GpioOutput motor2INB(GPIOG, GPIO_PIN_14);  /* D2 starts LOW */
    static MotorDriver motor2(motor2PWM, motor2INA, motor2INB);

    /* PSC=0, ARR=3199 → 64MHz / 1 / 3200 = 20 kHz (~11.6-bit, 3200 steps)   */
    motor1PWM.start();
    motor2PWM.start();
    motor1PWM.setPwmRawValue(0);   /* example: ~50 % (1600 / 3200) */
    motor2PWM.setPwmRawValue(0);

    /* ── User LEDs (Nucleo-H723ZG MB1364) ──────────────────────────────────── */
    static GpioOutput greenLedPin (GPIOB, GPIO_PIN_0);   /* LD1 — PB0  */
    //static GpioOutput yellowLedPin(GPIOE, GPIO_PIN_1);   /* LD2 — PE1  */
    //static GpioOutput redLedPin   (GPIOB, GPIO_PIN_14);  /* LD3 — PB14 */
    static BlinkyFreeRTOSTask greenLed(greenLedPin, 1000);  /* 1 Hz       */



    greenLed.start("GreenLED", 1);
    hostHandler.start(1, 512);  /* 512 words — extra headroom for float printf */

    vTaskStartScheduler();
    while (true);
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

/**
 * HAL MSP override: configure GPIO pins for TIM1 PWM channels.
 *   TIM1_CH1 → PE9  (AF1)   Arduino D6 (CN10)
 *   TIM1_CH2 → PE11 (AF1)   Arduino D5 (CN10)
 * Called automatically by HAL_TIM_PWM_Init().
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim) {
    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();

        GPIO_InitTypeDef gpio = {0};
        gpio.Pin       = GPIO_PIN_9 | GPIO_PIN_11;
        gpio.Mode      = GPIO_MODE_AF_PP;
        gpio.Pull      = GPIO_NOPULL;
        gpio.Speed     = GPIO_SPEED_FREQ_LOW;
        gpio.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOE, &gpio);
    }
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim) {
    if (htim->Instance == TIM1) {
        __HAL_RCC_TIM1_FORCE_RESET();
        __HAL_RCC_TIM1_RELEASE_RESET();
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_9 | GPIO_PIN_11);
    }
}

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
