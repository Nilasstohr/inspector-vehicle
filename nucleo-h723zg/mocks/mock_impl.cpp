// Fake implementations of HAL and FreeRTOS functions used during unit tests.
// Tests inspect the spy variables below to verify behaviour.
#include "stm32h7xx_hal.h"
#include "task.h"

// ── UART flag mock ───────────────────────────────────────────────────────────
FlagStatus mock_uart_rxne_flag = RESET;  // default: no data available

// ── HAL spy ──────────────────────────────────────────────────────────────────
GPIO_TypeDef* g_lastToggledPort = nullptr;
uint16_t g_lastToggledPin = 0;

void HAL_GPIO_TogglePin(GPIO_TypeDef* port, uint16_t pin) {
    g_lastToggledPort = port;
    g_lastToggledPin = pin;
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef* /*huart*/, const uint8_t* /*pData*/,
                                    uint16_t /*Size*/, uint32_t /*Timeout*/) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef* /*huart*/, const uint8_t* /*pData*/,
                                    uint16_t /*Size*/, uint32_t /*Timeout*/) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef* /*huart*/, uint8_t* /*pData*/,
                                            uint16_t /*Size*/, uint16_t* RxLen, uint32_t /*Timeout*/) {
    if (RxLen) *RxLen = 0;
    return HAL_OK;
}

// ── FreeRTOS spies ───────────────────────────────────────────────────────────
bool g_xTaskCreateCalled = false;
const char* g_xTaskCreateName = nullptr;
UBaseType_t g_xTaskCreatePriority = 0;
TickType_t g_lastVTaskDelay = 0;

BaseType_t xTaskCreate(TaskFunction_t fn, const char* name, uint16_t /*stackDepth*/, void* params,
                       UBaseType_t priority, TaskHandle_t* handle) {
    g_xTaskCreateCalled = true;
    g_xTaskCreateName = name;
    g_xTaskCreatePriority = priority;
    (void)fn;
    (void)params;
    if (handle) *handle = reinterpret_cast<TaskHandle_t>(1);
    return pdTRUE;
}

void vTaskDelay(TickType_t ticks) { g_lastVTaskDelay = ticks; }

BaseType_t xTaskGetSchedulerState(void) { return taskSCHEDULER_NOT_STARTED; }
