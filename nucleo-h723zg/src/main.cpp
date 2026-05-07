/**
 * @file    main.cpp
 * @brief   FreeRTOS blinky for Nucleo-H723ZG (C++)
 */

#include <gpio/GpioOutput.h>
#include <host_command_handler/HostCommandHandlerFreeTOSTask.h>
#include <motors/TelemetryTask.h>
#include <motors/Encoder.h>
#include <motors/MotorDriver.h>
#include <motors/MotorPinConfig.h>
#include <motors/PiMotorControl.h>
#include <pwm/PwmOutput.h>
#include <timer/HardwareTimer.h>
#include <uart/UartLineReceiver.h>
#include <uart/UartTransceiver.h>

#include "BlinkyFreeRTOSTask.h"
#include "CrashHandler.h"
#include "FreeRTOS.h"
#include "stm32h7xx_hal.h"
#include "task.h"
#include "utils/DwtTimer.h"

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
    DwtTimer::init();   /* enable µs cycle counter — must be before any Encoder use */
    UART3_Init();


    // Motor 1
    static PwmOutput motor1PWM(TIM1, TIM_CHANNEL_2);  /* PE11 — TIM1_CH2 (AF1) — D5 (CN10) */
    static GpioOutput motor1INA(MOTOR1_INA_PORT, MOTOR1_INA_PIN);  /* D3  starts LOW */
    static GpioOutput motor1INB(MOTOR1_INB_PORT, MOTOR1_INB_PIN);  /* D2 starts LOW */
    static MotorDriver motor1Driver(motor1PWM, motor1INA, motor1INB);
    static GpioInput motor1EncA(MOTOR1_ENC_A_PORT, MOTOR1_ENC_A_PIN, GPIO_PULLUP, GPIO_MODE_IT_RISING_FALLING);
    static GpioInput motor1EncB(MOTOR1_ENC_B_PORT, MOTOR1_ENC_B_PIN, GPIO_PULLUP, GPIO_MODE_IT_RISING_FALLING);
    static Encoder motor1Encoder(motor1EncA, motor1EncB);
    Encoder::registerInstance(0, motor1Encoder);  /* register BEFORE enabling IRQs */
    /* Each encoder pin is on its own dedicated EXTI line (1 and 2). */
    HAL_NVIC_SetPriority(MOTOR1_ENC_A_IRQn, MOTOR1_ENC_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(MOTOR1_ENC_A_IRQn);
    HAL_NVIC_SetPriority(MOTOR1_ENC_B_IRQn, MOTOR1_ENC_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(MOTOR1_ENC_B_IRQn);

    // Motor 2
    static PwmOutput motor2PWM (TIM1, TIM_CHANNEL_1);  /* PE9  —  D6 (CN10) */
    static GpioOutput motor2INA(MOTOR2_INA_PORT, MOTOR2_INA_PIN);  /*  D7 starts LOW */
    static GpioOutput motor2INB(MOTOR2_INB_PORT, MOTOR2_INB_PIN);  /*  D4 starts LOW */
    static MotorDriver motor2Driver(motor2PWM, motor2INA, motor2INB);

    static GpioInput motor2EncA(MOTOR2_ENC_A_PORT, MOTOR2_ENC_A_PIN, GPIO_PULLUP, GPIO_MODE_IT_RISING_FALLING);
    static GpioInput motor2EncB(MOTOR2_ENC_B_PORT, MOTOR2_ENC_B_PIN, GPIO_PULLUP, GPIO_MODE_IT_RISING_FALLING);
    static Encoder motor2Encoder(motor2EncA, motor2EncB);
    Encoder::registerInstance(1, motor2Encoder);  /* register BEFORE enabling IRQs */
    /* Each encoder pin is on its own dedicated EXTI line (1 and 2). */
    HAL_NVIC_SetPriority(MOTOR2_ENC_A_IRQn, MOTOR2_ENC_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(MOTOR2_ENC_A_IRQn);
    HAL_NVIC_SetPriority(MOTOR2_ENC_B_IRQn, MOTOR2_ENC_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(MOTOR2_ENC_B_IRQn);


    /* PSC=0, ARR=3199 → 64MHz / 1 / 3200 = 20 kHz (~11.6-bit, 3200 steps)   */
    motor1PWM.start();
    motor2PWM.start();
    motor1Driver.setForward();
    motor2Driver.setForward();
    motor1PWM.setPwmRawValue(0);   /* example: ~50 % (1600 / 3200) */
    motor2PWM.setPwmRawValue(0);

    /* ── User test pin──────────────────────────────────── */
    static GpioOutput timingTestPin (GPIOA, GPIO_PIN_9);   /* LD1 — PB0  */

    /* ── User LEDs (Nucleo-H723ZG MB1364) ──────────────────────────────────── */
    static GpioOutput greenLedPin (GPIOB, GPIO_PIN_0);   /* LD1 — PB0  */
    //static GpioOutput yellowLedPin(GPIOE, GPIO_PIN_1);   /* LD2 — PE1  */
    //static GpioOutput redLedPin   (GPIOB, GPIO_PIN_14);  /* LD3 — PB14 */

    static auto uartTransceiver = UartTransceiver(huart3);
    static Uart uart(uartTransceiver);
    static UartLineReceiver uartLineReceiver(huart3);
    UartLineReceiver::setInstance(&uartLineReceiver);
    /* Report any fault saved from the previous run before starting tasks */
    CrashHandler_checkAndReport(&uart);

    static PiMotorControl   piMotorControl(
        motor2Encoder,
        motor1Encoder,
        motor2Driver,
        motor1Driver,
        timingTestPin);

    static HardwareTimer timer;
    PiMotorControl::registerInstance(piMotorControl);  /* register BEFORE enabling IRQ */
    timer.start();

    /* ── FreeTOSTasks ──────────────────────────────────── */
    static HostCommandHandlerFreeTOSTask hostHandler(uart,uartLineReceiver,piMotorControl);
    static BlinkyFreeRTOSTask greenLed(greenLedPin, 1000);  /* 1 Hz       */
    /* TelemetryTask runs at the lowest priority so UART blocking never
     * interferes with the ISR controller loop.  200 ms period → 5 lines/s. */
    //static TelemetryTask telemetry(uart, piMotorControl, /*periodMs=*/200U);
    //greenLed.start(1, "GreenLED", 512);
    hostHandler.start(1, "HostCmdHandler", 512);  /* priority 1 — low priority for UART I/O */
    //telemetry.start(1, "Telemetry", 512);  /* priority 1 — low priority for UART I/O */
    vTaskStartScheduler();
    /* Unreachable — scheduler never returns on a correctly configured system */
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

    /* Enable the receive interrupt permanently — USART3_IRQHandler reads RDR
   * directly, so no HAL_UART_Receive_IT / re-arming is ever needed.
   * RXNEIE_RXFNEIE fires for every byte whether FIFO is enabled or not. */
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);

    /* Priority 6 — inside the FreeRTOS managed zone (≥ configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5) */
    HAL_NVIC_SetPriority(USART3_IRQn, /*preempt=*/6, /*sub=*/0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
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
