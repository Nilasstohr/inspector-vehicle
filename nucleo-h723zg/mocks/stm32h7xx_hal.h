#pragma once
// Host-only stub — replaces the real STM32 HAL header during unit tests.
#include <stdint.h>

typedef uint32_t HAL_StatusTypeDef;
#define HAL_OK      ((HAL_StatusTypeDef)0)
#define HAL_ERROR   ((HAL_StatusTypeDef)1)
#define HAL_BUSY    ((HAL_StatusTypeDef)2)
#define HAL_TIMEOUT ((HAL_StatusTypeDef)3)

typedef struct {
    uint32_t dummy;
} GPIO_TypeDef;

typedef struct {
    uint32_t dummy;
} UART_HandleTypeDef;

void HAL_GPIO_TogglePin(GPIO_TypeDef* port, uint16_t pin);
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef* huart, const uint8_t* pData, uint16_t Size,
                                    uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef* huart, const uint8_t* pData, uint16_t Size,
                                   uint32_t Timeout);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef* huart, uint8_t* pData, uint16_t Size,
                                            uint16_t* RxLen, uint32_t Timeout);

// Flag / status mock stubs
typedef uint32_t FlagStatus;
#define RESET ((FlagStatus)0)
#define SET   ((FlagStatus)1)

#define UART_FLAG_RXNE  (0x0020U)

// Stub: always reports no data available; tests can override via mock_impl
extern FlagStatus mock_uart_rxne_flag;
#define __HAL_UART_GET_FLAG(__HANDLE__, __FLAG__) \
    ((__FLAG__) == UART_FLAG_RXNE ? mock_uart_rxne_flag : RESET)

