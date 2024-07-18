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

static TaskHandle_t revieve_data_handle = NULL;
//TODO: ADD STRUCT FOR DATA FOR THERMOCOUPLES AND 1 QUEUE FOR DATA STRUCT NOT FOR 2 THERMOCOUPLES
esp_err_t measure_task(void *pvParameters)
{
    ESP_LOGI(TAG, "MEASURING TEMPERATURES");
    float ThermoTemp1=0;
    float ThermoTemp2=0;
    ThermoTemp1 = thermocouple_read_temperature(&TANWA_hardware.thermocouple[0]);
    ESP_LOGI(TAG, "ThermoTemp1 = : %0.2f", ThermoTemp1);
    ThermoTemp2 = thermocouple_read_temperature(&TANWA_hardware.thermocouple[1]);
    ESP_LOGI(TAG, "ThermoTemp2 = : %0.2f", ThermoTemp2);

    if(xQueueSend(ThermoTemp_queue, &ThermoTemp1, 0) != pdTRUE){
        ESP_LOGE(TAG, "Error while sending ThermoTemp1 to queue");
    }

    if(xQueueSend(ThermoTemp_queue, &ThermoTemp2, 0) != pdTRUE){
        ESP_LOGE(TAG, "Error while sending ThermoThmp2 to queue");
    }
    
    ESP_LOGI(TAG, "MEASURED THERMOCOUPLES");

    return ESP_OK;
}



void decode_command(can_flc_commands_t *cmd)
{
    esp_err_t ret;
    if(cmd == NULL)
    {
        ESP_LOGE(TAG, "Command error");
        return;
    }
    switch(*cmd)
    {
        case CAN_FLC_TX_GET_STATUS:
            ESPLOGI(TAG,"STATUS CMD");
            ret = measure_task();
            if(ret!=ESP_OK)
            {
                set_tatus(FLC_STATUS_ERROR);
            }
            else
            set_tatus(FLC_STATUS_OK);
            break;
        case CAN_FLC_TX_GET_DATA:
            ESPLOGI(TAG,"DATA CMD");
            break;
        default:
            ESP_LOGE(TAG, "Unknown command");
            break;
    }
}
void recieve_data_task(void *pvParameters)
{
     ESP_LOGI(TAG, "Recieving task is running");
     can_flc_commands_t command;
     TickType_t xLastTime = xTaskGetTickCount();
    while (1) 
    {

        if(xQueueReceive(CMDS_queue, &command, pdMS_TO_TICKS(10)) != pdTRUE){
            ESP_LOGE(TAG, "Error while recieving command from queue");
        }

        decode_command(&command);
        command = CAN_FLC_GET_DATA;
        if(xQueueSend(CMDS_queue,&command,pdMS_TO_TICKS(10))!=pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to run get data command");
        }

        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(100));
    }
}

void stop_measure_task(void) 
{
    if (measure_task_handle != NULL) 
    {
        vTaskDelete(measure_task_handle);
        ESP_LOGI(TAG, "### measure_task deleted ###");
        measure_task_handle = NULL;
    }
    else 
    {
        ESP_LOGE(TAG, "### measure_task handle is NULL or task already deleted ###");
    }
}

void run_recieve_task(void) {
    xTaskCreatePinnedToCore(recieve_data_task, "recieve_data_task", 4096, NULL, 1,
                            &revieve_data_handle, 0);
}

