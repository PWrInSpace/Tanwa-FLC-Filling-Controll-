///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 21.06.2024 by Mateusz Kłosiński
///
///===-----------------------------------------------------------------------------------------===//
#include "data_handling.h"
#define TAG "DATA_TASK"

/**
 * @brief function that measures the thermocuoples temperatures
 * 
 * @return esp_err_t 
 */
esp_err_t measure()
{
    ESP_LOGI(TAG, "MEASURING TEMPERATURES");

    int16_t thermo_temp_cj[MAX31856_QUANTITY] = {0};
    int16_t thermo_temp[MAX31856_QUANTITY] = {0};

    // Measure cold junction and temperature for each thermocouple and sends it to queue
    for (int i = 0; i < MAX31856_QUANTITY; i++)
    {
        thermo_temp_cj[i] = ((int16_t)thermocouple_read_coldjunction(&TANWA_hardware.thermocouple[i]));
        thermo_temp[i] = ((int16_t)thermocouple_read_temperature(&TANWA_hardware.thermocouple[i]));

        ESP_LOGI(TAG, "thermo_temp_cj[%d] = : %d", i, thermo_temp_cj[i]);
        ESP_LOGI(TAG, "thermo_temp[%d] = : %d", i, thermo_temp[i]);
    }

    // Overwrite the queues with the latest data
    xQueueOverwrite(ThermoTemp_queue_cj, &thermo_temp_cj);
    xQueueOverwrite(ThermoTemp_queue, &thermo_temp);

    ESP_LOGI(TAG, "MEASURED THERMOCOUPLES");

    return ESP_OK;
}

/**
 * @brief function that measures pressure and sends it to queue
 * 
 * @return esp_err_t 
 */
esp_err_t measure_pressure()
{
    ESP_LOGI(TAG, "MEASURING PRESSURE");

    float pressure[PRESSURE_DRIVER_SENSOR_COUNT] = {0};

    // Measure pressure from each sensor
    pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_1, &pressure[0]);
    pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &pressure[1]);
    pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_3, &pressure[2]);
    pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_4, &pressure[3]);

    int16_t pressure_parsed[PRESSURE_DRIVER_SENSOR_COUNT] = {0};
    for (int i = 0; i < PRESSURE_DRIVER_SENSOR_COUNT; i++)
    {
        pressure_parsed[i] = pressure[i];
    }
    // Overwrite the queue with the latest set of pressure data
    xQueueOverwrite(PressureSens, &pressure_parsed);

    // Log each pressure reading
    for (int i = 0; i < PRESSURE_DRIVER_SENSOR_COUNT; i++)
    {
        ESP_LOGI(TAG, "PRESSURE[%d] = : %d", i, pressure_parsed[i]);
    }

    ESP_LOGI(TAG, "MEASURED PRESSURE");

    return ESP_OK;
}
/**
 * @brief function that decodes command recieved from cmds_queue
 * 
 * @param cmd 
 */
void decode_command(can_flc_commands_t *cmd)
{
    if (cmd == NULL)
    {
        ESP_LOGE(TAG, "Command error");
        return;
    }
    switch (*cmd)
    {
    case CAN_FLC_RX_GET_DATA:
    {
        ESP_LOGI(TAG, "GET_DATA CMD");
        if (measure() != ESP_OK)
        {
            set_status(FLC_STATUS_ERROR);
        }
        set_status(FLC_STATUS_OK);
    }
    break;

    case CAN_FLC_RX_SOFT_RESET:
    {
        esp_restart();
    }
    break;

    case CAN_FLC_RX_GET_DATA_PRESSURE:
    {
        ESP_LOGI(TAG, "GET_DATA PRESSURE CMD");
        if (measure_pressure() != ESP_OK)
        {
            set_status(FLC_STATUS_ERROR);
        }
        set_status(FLC_STATUS_OK);
    }
    break;

    default:
    {
        ESP_LOGE(TAG, "Unknown command");
    }
    break;
    }
}
/**
 * @brief function that handles the commands given to queue with given delay
 * 
 * @param pvParameters 
 */
void data_handle_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Recieving task is running");
    can_flc_commands_t command;
    TickType_t xLastTime = xTaskGetTickCount();
    float temperature_celsius[TMP1075_QUANTITY] = {0};
    while (1)
    {
        command = CAN_FLC_RX_GET_DATA;
        if (xQueueSend(CMDS_queue, &command, pdMS_TO_TICKS(100)) != pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to run get data command");
        }
        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(1000));

        command = CAN_FLC_RX_GET_DATA_PRESSURE;
        if (xQueueSend(CMDS_queue, &command, pdMS_TO_TICKS(100)) != pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to run get data pressure command");
        }

        if (xQueueReceive(CMDS_queue, &command, pdMS_TO_TICKS(100)) != pdTRUE)
        {
            ESP_LOGE(TAG, "Error while recieving command from queue");
        }
        decode_command(&command);

        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(1000));
        
        if (tmp1075_get_temp_celsius(&TANWA_hardware.tmp1075[0], &temperature_celsius[0]) != ESP_OK)
        {
            ESP_LOGE(TAG, "TEMP READ CELSIUS FAIL");
        }
        ESP_LOGI(TAG,"TMP1075 TEMP IS : %f", temperature_celsius[0]);

        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(1000));
    }
}
/**
 * @brief function that stops the data_task()
 * 
 */
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

/**
 * @brief function that creates the data_handle_task task
 * 
 */
void run_data_handling_task(void)
{
    xTaskCreatePinnedToCore(data_handle_task, "data_handle_task", 4096, NULL, 1,
                            &data_handle_handle, 0);
}
