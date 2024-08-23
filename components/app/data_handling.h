///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 21.06.2024 by Mateusz Kłosiński
///
///===-----------------------------------------------------------------------------------------===//


/**
 * 
 */
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "can_commands.h"
#include "rtos_utilities.h"
#include "TANWA_FLC_config.h"
#define TAG "MEASURE_TASK"

static TaskHandle_t data_handle_handle = NULL;
//TODO: ADD STRUCT FOR DATA FOR THERMOCOUPLES AND 1 QUEUE FOR DATA STRUCT NOT FOR 2 THERMOCOUPLES
esp_err_t measure();

void decode_command(can_flc_commands_t *cmd);


void data_handle_task(void *pvParameters);

void stop_measure_task(void);


void run_recieve_task(void);
