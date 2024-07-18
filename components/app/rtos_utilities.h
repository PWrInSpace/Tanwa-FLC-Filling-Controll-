///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 21.06.2024 by Mateusz Kłosiński
///
///===-----------------------------------------------------------------------------------------===//

#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "can_commands.h"
#include "esp_log.h"

#define FLC_STATUS_OK 0
#define FLC_STATUS_ERROR 1
extern QueueHandle_t CMDS_queue;
extern QueueHandle_t ThermoTemp_queue;

esp_err_t rtos_util_init(void *pvParameters);
