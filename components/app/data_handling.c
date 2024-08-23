///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 21.06.2024 by Mateusz Kłosiński
///
///===-----------------------------------------------------------------------------------------===//
#include "data_handling.h"

esp_err_t measure()
{   
    ESP_LOGI(TAG, "MEASURING TEMPERATURES");

    float thermo_temp_cj[MAX31856_QUANTITY] = {0};
    float thermo_temp[MAX31856_QUANTITY] = {0};
    for (int i=0; i< MAX31856_QUANTITY; i++)
    {
    thermo_temp_cj[i]= thermocouple_read_coldjunction(&TANWA_hardware.thermocouple[i]);
    thermo_temp[i] = thermocouple_read_temperature(&TANWA_hardware.thermocouple[i]);
    ESP_LOGI(TAG, "thermo_temp_cj = : %0.2f", thermo_temp_cj);
    ESP_LOGI(TAG, "thermo_temp = : %0.2f", thermo_temp);

     if(xQueueSend(ThermoTemp_queue_cj, &thermo_temp_cj[i], 0) != pdTRUE){
        ESP_LOGE(TAG, "Error while sending thermo_temp_cj[%d] to queue", i);
    }

    if(xQueueSend(ThermoTemp_queue, &thermo_temp[i], 0) != pdTRUE){
        ESP_LOGE(TAG, "Error while sending thermo_temp[%d] to queue", i);
    }

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
        case CAN_FLC_RX_GET_DATA:
        {
            ESP_LOGI(TAG,"GET_DATA CMD");
            if(measure()!=ESP_OK)
            {
                set_status(FLC_STATUS_ERROR);
            }
                set_status(FLC_STATUS_OK);
        }break;

        case CAN_FLC_RX_SOFT_RESET:
        {
            esp_restart();
        }break;

        default:
        {
            ESP_LOGE(TAG, "Unknown command");
        }break;
    }
}


void data_handle_task(void *pvParameters)
{
     ESP_LOGI(TAG, "Recieving task is running");
     can_flc_commands_t command;
     TickType_t xLastTime = xTaskGetTickCount();
    while (1) 
    {

        if(xQueueReceive(CMDS_queue, &command, pdMS_TO_TICKS(10)) != pdTRUE)
        {
            ESP_LOGE(TAG, "Error while recieving command from queue");
        }
        decode_command(&command);

        command = CAN_FLC_RX_GET_DATA;
        if(xQueueSend(CMDS_queue,&command,pdMS_TO_TICKS(10))!=pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to run get data command");
        }

        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(100));
    }
}

void stop_data_task(void) 
{
    if (data_handle_handle != NULL) 
    {
        vTaskDelete(data_handle_handle);
        ESP_LOGI(TAG, "### data task deleted ###");
        data_handle_handle = NULL;
    }
    else 
    {
        ESP_LOGE(TAG, "### data task handle is NULL or task already deleted ###");
    }
}

void run_recieve_task(void) {
    xTaskCreatePinnedToCore(data_handle_task, "data_handle_task", 4096, NULL, 1,
                            &data_handle_handle, 0);
}
