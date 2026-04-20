#pragma once
// Host-only stub — replaces the real FreeRTOS.h during unit tests.
#include <stdint.h>

typedef uint32_t TickType_t;
typedef long BaseType_t;
typedef uint32_t UBaseType_t;
typedef void* TaskHandle_t;

#define pdMS_TO_TICKS(ms) ((TickType_t)(ms))
#define pdTRUE ((BaseType_t)1)
#define pdFALSE ((BaseType_t)0)
#define portMAX_DELAY ((TickType_t)0xFFFFFFFFU)
