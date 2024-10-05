///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 31.05.2024 by Mateusz Kłosiński
///===-----------------------------------------------------------------------------------------===//

#include "can_task.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "mcu_twai_config.h"

#include "can_commands.h"
#include "rtos_utilities.h"
#include "TANWA_FLC_config.h"

#define CAN_TASK_STACK_SIZE 4096
#define CAN_TASK_PRIORITY 9
#define CAN_TASK_CORE 1
#define CAN_TASK_DEFAULT_FREQ 50

#define TAG "TWAI Listen Only"

#ifdef master_conf

#endif

static TaskHandle_t can_task_handle = NULL;

TickType_t Max_time = 500;

/**
 * @brief function that inserts float into uint8_t array
 * 
 * @param value 
 * @param bytes 
 * @param start_idx 
 * @return * void 
 */
void insert_float_into_uint8_array(float *value, uint8_t *bytes, uint8_t start_idx)
{
    if (start_idx > 4)
    {
        ESP_LOGE(TAG, "Invalid start index");
        return;
    }
    memcpy(bytes + start_idx, value, sizeof(float));
}
/**
 * @brief functiion that inserts int16_t into uint8_t array
 * 
 * @param value 
 * @param bytes 
 * @param start_idx 
 */
void insert_int16_t_into_uint8_array(int16_t *value, uint8_t *bytes, uint8_t start_idx)
{
    if (start_idx > 6)
    {
        ESP_LOGE(TAG, "Invalid start index");
        return;
    }
    memcpy(bytes + start_idx, value, sizeof(int16_t));
}
/**
 * @brief functiin that formats and sends FLC status with temp fro TMP1075
 * 
 */

void send_status(void)
{

    twai_message_t tx_msg;
    tx_msg.identifier = CAN_FLC_TX_STATUS;

    float temperature_celsius[TMP1075_QUANTITY] = {0};
    int16_t temp_parsed[TMP1075_QUANTITY] = {0};
    uint8_t data[8] = {0}; // Full 8-byte array to match tx_msg.data_length_code

    uint16_t status = get_status();
    memcpy(tx_msg.data, &status, sizeof(status));

    // Read temperature in Celsius
    for (int i = 0; i < TMP1075_QUANTITY; i++)
    {
        if (tmp1075_get_temp_celsius(&TANWA_hardware.tmp1075[i], &temperature_celsius[i]) != ESP_OK)
        {
            ESP_LOGE(TAG, "TEMP READ CELSIUS FAIL");
            return;
        }
    }

    for (int i = 0; i < TMP1075_QUANTITY; i++)
    {
        temp_parsed[i] = temperature_celsius[i];
    }

    // Insert the int16_t value into the data array starting at index x

    int i = 0;
    while (i < TMP1075_QUANTITY)
    {
        for (int j = sizeof(status); j < 4; j += 2)
        {
            if (i >= TMP1075_QUANTITY)
            break;
            insert_int16_t_into_uint8_array(&temp_parsed[i], data, j);
            i++;
        }

        if (i >= TMP1075_QUANTITY)
            break;
        
    }

    // manually input//insert_int16_t_into_uint8_array(&temp_parsed[0], data, 2);

    tx_msg.data_length_code = 8; // Full length

    // Copy the data array into the tx_msg.data
    memcpy(tx_msg.data, data, tx_msg.data_length_code);

    // Transmit the message
    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) != ESP_OK)
    {
        ESP_LOGE(TAG, "TRANSMIT TEMP CELSIUS FAIL");
        return;
    }
}
/**
 * @brief function that formats and sends the thermocouple data
 * 
 */
void send_data(void)

{

    //  INIT DATA
    twai_message_t tx_msg;
    tx_msg.identifier = CAN_FLC_TX_DATA;
    tx_msg.data_length_code = 8;
    uint8_t data[8] = {0};
    int16_t temp[MAX31856_QUANTITY] = {0};
    int16_t temp_cj[MAX31856_QUANTITY] = {0};

    // RECIEVE FROM QUEUE
    xQueueReceive(ThermoTemp_queue, &temp, 100);
    printf("##########################################\n");
    ESP_LOGI(TAG,"GOWNO[0] %d   GOWNO[1] %d     GOWNO[2] %d     GOWNO[3] %d\n", temp[0],data[1],data[2],data[3]);
    printf("##########################################\n");
   // xQueueReceive(ThermoTemp_queue_cj, &temp_cj, 0); //not used

    //*INPUT TO FRAME
    int i = 0;
    while (i < MAX31856_QUANTITY)
    {
    // Loop to insert int16_t values into the uint8_t array
    for (int j = 0; j < 4; j += 2)
    {
        if (i >= MAX31856_QUANTITY)  // Make sure we don't go out of bounds
            break;
        
        insert_int16_t_into_uint8_array(&temp[i], data, j);
        i++;  // Move to the next int16_t value
    }

    if (i >= MAX31856_QUANTITY)  // Double-check bounds to avoid overflow
        break;
    }
    //*END INPUT TO FRAME
    

    memcpy(tx_msg.data, data, tx_msg.data_length_code);

    // Transmit the message
    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) != ESP_OK)
    {
        ESP_LOGE(TAG, "TRANSMIT ThermoCouple FAIL");
        return;
    }
}

/**
 * @brief function that formats and sends the pressure data
 * 
 */
void send_data_pressure(void)

{
    twai_message_t tx_msg;
    tx_msg.identifier = CAN_FLC_TX_DATA_PRESSURE;
    tx_msg.data_length_code = 8;

    uint8_t data[8] = {0};
    int16_t pressure[4] = {0};

    xQueueReceive(PressureSens, &pressure, 100);
    
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    ESP_LOGI(TAG,"PRESSURE[0] %d   PRESSURE[1] %d     PRESSURE[2] %d     PRESSURE[3] %d\n", pressure[0],pressure[1],pressure[2],pressure[3]);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    
   //*INPUT TO FRAME
    int i = 0;
    while (i < PRESSURE_DRIVER_SENSOR_COUNT)
    {
    // Loop to insert int16_t values into the uint8_t array
    for (int j = 0; j < 8; j += 2)
    {
        if (i >= PRESSURE_DRIVER_SENSOR_COUNT)  // Make sure we don't go out of bounds
            break;
        
        insert_int16_t_into_uint8_array(&pressure[i], data, j);
        i++;  // Move to the next int16_t value
    }

    if (i >= PRESSURE_DRIVER_SENSOR_COUNT)  // Double-check bounds to avoid overflow
        break;
    }

    
     

    memcpy(tx_msg.data, data, tx_msg.data_length_code);


   

    
    // Transmit the message
    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) != ESP_OK)
    {
        ESP_LOGE(TAG, "TRANSMIT ThermoCouple FAIL");
        return;
    }
}
void update_state()
{

    twai_message_t tx_msg;
    tx_msg.identifier = CAN_FLC_TX_BEACON;
    tx_msg.data_length_code = 8;

        if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) != ESP_OK)
        {
        ESP_LOGE(TAG, "TRANSMIT Beacon FAIL");
        return;
        }
    
}

/**
 * @brief function that decodes the message
 * 
 * @param rx_msg // inputed message from CAN
 */
void can_decode_message(twai_message_t rx_msg)
{
    switch (rx_msg.identifier)
    {
    case CAN_FLC_RX_GET_STATUS:
    {
        ESP_LOGI(TAG, "FLC RX SEND_STATUS");
        send_status();
    }
    break;

    case CAN_FLC_RX_GET_DATA:
    {
        ESP_LOGI(TAG, "FLC RX GET_THERMOCOUPLES");
        send_data();
    }
    break;

    case CAN_FLC_RX_GET_DATA_PRESSURE:
    {
        ESP_LOGI(TAG, "FLC RX GET_PRESSURE");
        send_data_pressure();
    }
    break;
    case CAN_FLC_RX_SOFT_RESET:
    {
        can_flc_commands_t cmd = CAN_FLC_RX_SOFT_RESET;
        if (xQueueSend(CMDS_queue, &cmd, 0) != pdTRUE)
        {
            ESP_LOGE(TAG, "TRANSMIT  CAN_FLC_TX_SOFT_RESET TO COMMANDS QUEUE FAIL");
        }
    }
    break;

    case CAN_FLC_RX_NOTHING:
    {
        __asm__ __volatile__("nop");
    }
    break;

    default:
        break;
    }
}

/**
 * @brief task function to handle can transcieving
 * 
 * @param pvParameters 
 */
void can_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t update_time = pdMS_TO_TICKS(1000);
    while (true)
    {
        twai_message_t rx_msg;
        if (twai_receive(&rx_msg, pdMS_TO_TICKS(100)) == ESP_OK)
        {
            can_decode_message(rx_msg);
        }
        TickType_t current_tick_count = xTaskGetTickCount();

         if ((current_tick_count - xLastWakeTime) >= update_time)
        {
            ESP_LOGI(TAG,"Update_state");
            update_state();
            xLastWakeTime = current_tick_count;
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CAN_TASK_DEFAULT_FREQ));
    }
    vTaskDelete(NULL);
}

/**
 * @brief function that starts the can_task() task
 * 
 */
void run_can_task(void)
{
    if (twai_start() != ESP_OK) {
      ESP_LOGE(TAG, "TWAI start error");
    } else {
    xTaskCreatePinnedToCore(can_task, "can_task", CAN_TASK_STACK_SIZE, NULL, CAN_TASK_PRIORITY,
                            &can_task_handle, CAN_TASK_CORE);
    //xTaskCreatePinnedToCore(send_beacon_task, "send_beacon_task", 1024, NULL, CAN_TASK_PRIORITY-1,
    //                        &send_beacon_task_handle, CAN_TASK_CORE-1);
    }

}
