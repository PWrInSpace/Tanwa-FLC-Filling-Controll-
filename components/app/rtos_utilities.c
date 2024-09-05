///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 21.06.2024 by Mateusz Kłosiński
///
///===-----------------------------------------------------------------------------------------===//
#include "esp_log.h"
#include "rtos_utilities.h"
#include "can_commands.h"
#define TAG "RTOS_UTILITIES"

static SemaphoreHandle_t mutex;

volatile bool FLC_status;

QueueHandle_t CMDS_queue = NULL;
QueueHandle_t ThermoTemp_queue = NULL;
QueueHandle_t ThermoTemp_queue_cj = NULL;
QueueHandle_t PressureSens = NULL;

esp_err_t rtos_util_init()
{
    mutex = xSemaphoreCreateMutex();
    FLC_status = FLC_STATUS_OK;

    CMDS_queue = xQueueCreate(10, sizeof(can_flc_commands_t));
    if (CMDS_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create CMDS_queue");
        return ESP_FAIL;
    }

    ThermoTemp_queue = xQueueCreate(1, MAX31856_QUANTITY * sizeof(int16_t));
    if (ThermoTemp_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create ThermoTemp_queue");
        return ESP_FAIL;
    }
    ThermoTemp_queue_cj = xQueueCreate(1, MAX31856_QUANTITY * sizeof(int16_t));
    if (ThermoTemp_queue_cj == NULL) {
        ESP_LOGE(TAG, "Failed to create ThermoTemp_queue_cj_queue");
        return ESP_FAIL;
    }
    PressureSens = xQueueCreate(1, PRESSURE_DRIVER_SENSOR_COUNT * sizeof(int16_t));
    if (PressureSens == NULL) {
        ESP_LOGE(TAG, "Failed to create PressureSens_queue");
        return ESP_FAIL;
    }
    return ESP_OK;
}
/**
 * SETS BOARD STATUS
 */
void set_status(bool status)
{
    if(xSemaphoreTake(mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        FLC_status = status;
        xSemaphoreGive(mutex);
    }
}

bool get_status()
{   
    return FLC_status;
}