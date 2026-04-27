#pragma once
// Host-only stub — replaces the real FreeRTOS task.h during unit tests.
#include "FreeRTOS.h"

typedef void (*TaskFunction_t)(void*);

#define taskSCHEDULER_NOT_STARTED ((BaseType_t)1)
#define taskSCHEDULER_RUNNING ((BaseType_t)2)
#define taskSCHEDULER_SUSPENDED ((BaseType_t)0)

BaseType_t xTaskCreate(TaskFunction_t fn, const char* name, uint16_t stackDepth, void* params,
                       UBaseType_t priority, TaskHandle_t* handle);

void vTaskDelay(TickType_t ticks);

BaseType_t xTaskGetSchedulerState(void);
