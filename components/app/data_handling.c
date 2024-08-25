///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 21.06.2024 by Mateusz Kłosiński
///
///===-----------------------------------------------------------------------------------------===//
#include "data_handling.h"
#define TAG "DATA_TASK"
esp_err_t measure()
{   
    ESP_LOGI(TAG, "MEASURING TEMPERATURES");

    float thermo_temp_cj[MAX31856_QUANTITY] = {0};
    float thermo_temp[MAX31856_QUANTITY] = {0};
    for (int i=0; i< MAX31856_QUANTITY; i++)
    {
    thermo_temp_cj[i]= thermocouple_read_coldjunction(&TANWA_hardware.thermocouple[i]);
    thermo_temp[i] = thermocouple_read_temperature(&TANWA_hardware.thermocouple[i]);
    ESP_LOGI(TAG, "thermo_temp_cj[%d] = : %0.2f",i, thermo_temp_cj);
    ESP_LOGI(TAG, "thermo_temp[%d] = : %0.2f",i , thermo_temp);

     if(xQueueSend(ThermoTemp_queue_cj, &thermo_temp_cj[i], 0) != pdTRUE){
        ESP_LOGE(TAG, "Error while sending thermo_temp_cj to queue");
    }

    if(xQueueSend(ThermoTemp_queue, &thermo_temp[i], 0) != pdTRUE){
        ESP_LOGE(TAG, "Error while sending thermo_temp to queue");
    }

    }
    
    ESP_LOGI(TAG, "MEASURED THERMOCOUPLES");

    return ESP_OK;
}

esp_err_t measure_pressure()
{
    ESP_LOGI(TAG, "MEASURING PRESSURE");

    float pressure[PRESSURE_DRIVER_SENSOR_COUNT]= {0};
     // Measure pressure
             pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_1, &pressure[0]);
             pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &pressure[1]);
             pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_3, &pressure[2]);
             pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_4, &pressure[3]);

    for (int i=0; i< PRESSURE_DRIVER_SENSOR_COUNT; i++)
        {
            if(xQueueSend(PressureSens, &pressure[i], 0) != pdTRUE)
            {
        ESP_LOGE(TAG, "Error while sending pressure to queue");
            }
            ESP_LOGI(TAG, "PRESSURE[%d] = : %0.2f",i, pressure[i]);
        }

    ESP_LOGI(TAG, "MEASURED PRESSURE");

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

        case CAN_FLC_RX_GET_DATA_PRESSURE:
        {
            ESP_LOGI(TAG,"GET_DATA PRESSURE CMD");
            if(measure_pressure()!=ESP_OK)
            {
                set_status(FLC_STATUS_ERROR);
            }
                set_status(FLC_STATUS_OK);
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
        command = CAN_FLC_RX_GET_DATA;
        if(xQueueSend(CMDS_queue,&command,pdMS_TO_TICKS(2500))!=pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to run get data command");
        }
        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(1000));

         command = CAN_FLC_RX_GET_DATA_PRESSURE;
         if(xQueueSend(CMDS_queue,&command,pdMS_TO_TICKS(2500))!=pdTRUE)
         {
             ESP_LOGE(TAG, "Failed to run get data command");
         }

        
 
        if(xQueueReceive(CMDS_queue, &command, pdMS_TO_TICKS(2500)) != pdTRUE)
        {
            ESP_LOGE(TAG, "Error while recieving command from queue");
        }
        decode_command(&command);

        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(2500));
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

void run_data_handling_task(void) {
    xTaskCreatePinnedToCore(data_handle_task, "data_handle_task", 4096, NULL, 1,
                            &data_handle_handle, 0);
}
