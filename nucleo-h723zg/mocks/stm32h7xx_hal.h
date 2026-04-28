#pragma once
// Host-only stub — replaces the real STM32 HAL header during unit tests.
#include <stdint.h>

typedef uint32_t HAL_StatusTypeDef;
#define HAL_OK      ((HAL_StatusTypeDef)0)
#define HAL_ERROR   ((HAL_StatusTypeDef)1)
#define HAL_BUSY    ((HAL_StatusTypeDef)2)
#define HAL_TIMEOUT ((HAL_StatusTypeDef)3)

typedef struct { uint32_t dummy; } GPIO_TypeDef;
typedef struct { uint32_t dummy; } UART_HandleTypeDef;

typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET   = 1
} GPIO_PinState;

/* GPIO init struct stub */
typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
} GPIO_InitTypeDef;

/* GPIO mode / pull / speed constants */
#define GPIO_MODE_OUTPUT_PP   (0x00000001U)
#define GPIO_NOPULL           (0x00000000U)
#define GPIO_SPEED_FREQ_LOW   (0x00000000U)

/* Fake port instances — just distinct non-null addresses */
extern GPIO_TypeDef _mock_GPIOA, _mock_GPIOB, _mock_GPIOC, _mock_GPIOD;
extern GPIO_TypeDef _mock_GPIOE, _mock_GPIOF, _mock_GPIOG, _mock_GPIOH;
extern GPIO_TypeDef _mock_GPIOJ, _mock_GPIOK;
#define GPIOA (&_mock_GPIOA)
#define GPIOB (&_mock_GPIOB)
#define GPIOC (&_mock_GPIOC)
#define GPIOD (&_mock_GPIOD)
#define GPIOE (&_mock_GPIOE)
#define GPIOF (&_mock_GPIOF)
#define GPIOG (&_mock_GPIOG)
#define GPIOH (&_mock_GPIOH)
#define GPIOJ (&_mock_GPIOJ)
#define GPIOK (&_mock_GPIOK)

/* RCC clock-enable no-ops */
#define __HAL_RCC_GPIOA_CLK_ENABLE()  ((void)0)
#define __HAL_RCC_GPIOB_CLK_ENABLE()  ((void)0)
#define __HAL_RCC_GPIOC_CLK_ENABLE()  ((void)0)
#define __HAL_RCC_GPIOD_CLK_ENABLE()  ((void)0)
#define __HAL_RCC_GPIOE_CLK_ENABLE()  ((void)0)
#define __HAL_RCC_GPIOF_CLK_ENABLE()  ((void)0)
#define __HAL_RCC_GPIOG_CLK_ENABLE()  ((void)0)
#define __HAL_RCC_GPIOH_CLK_ENABLE()  ((void)0)
#define __HAL_RCC_GPIOJ_CLK_ENABLE()  ((void)0)
#define __HAL_RCC_GPIOK_CLK_ENABLE()  ((void)0)

/* GPIO HAL stubs */
void HAL_GPIO_Init(GPIO_TypeDef* port, GPIO_InitTypeDef* init);
void HAL_GPIO_DeInit(GPIO_TypeDef* port, uint32_t pin);
void HAL_GPIO_TogglePin(GPIO_TypeDef* port, uint16_t pin);
void HAL_GPIO_WritePin(GPIO_TypeDef* port, uint16_t pin, GPIO_PinState state);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* port, uint16_t pin);

/* UART HAL stubs */
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef* huart, const uint8_t* pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef* huart, const uint8_t* pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef* huart, uint8_t* pData, uint16_t Size, uint16_t* RxLen, uint32_t Timeout);

/* Flag / status mock stubs */
typedef uint32_t FlagStatus;
#define RESET ((FlagStatus)0)
#define SET   ((FlagStatus)1)

#define UART_FLAG_RXNE  (0x0020U)
#define UART_CLEAR_IDLEF  (0x00000010U)

extern FlagStatus mock_uart_rxne_flag;
#define __HAL_UART_GET_FLAG(__HANDLE__, __FLAG__) \
    ((__FLAG__) == UART_FLAG_RXNE ? mock_uart_rxne_flag : RESET)
#define __HAL_UART_CLEAR_FLAG(__HANDLE__, __FLAG__)  ((void)0)

