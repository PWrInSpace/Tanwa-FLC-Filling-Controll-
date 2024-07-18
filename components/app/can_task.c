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
#define CAN_TASK_DEFAULT_FREQ 100

#define TAG                             "TWAI Listen Only"

#ifdef master_conf

#endif

static SemaphoreHandle_t rx_sem;
static TaskHandle_t can_task_handle = NULL;



void insert_float_into_uint8_array(float *value, uint8_t *bytes, uint8_t start_idx) {
    if (start_idx > 4) { 
        ESP_LOGE(TAG, "Invalid start index");
        return;
    }
    memcpy(bytes + start_idx, value, sizeof(float));
}

void send_status(void) {
    float temperature_celsius = 0;
    uint8_t data[8] = {0}; // Full 8-byte array to match tx_msg.data_length_code
    twai_message_t tx_msg;
    tx_msg.identifier = CAN_FLC_TX_GET_STATUS;

    // Read temperature in Celsius
    if (tmp1075_get_temp_celsius(&TANWA_hardware.tmp1075[1], &temperature_celsius) != ESP_OK) {
        ESP_LOGE(TAG, "TEMP READ CELSIUS FAIL");
        return;
    }

    // Insert the float value into the data array starting at index 4
    insert_float_into_uint8_array(&temperature_celsius, data, 4);

    // Calculate the length of meaningful data
    tx_msg.data_length_code = 8; // Full length

    // Copy the data array into the tx_msg.data
    memcpy(tx_msg.data, data, tx_msg.data_length_code);

    // Transmit the message
    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) != ESP_OK) {
        ESP_LOGE(TAG, "TRANSMIT TEMP CELSIUS FAIL");
        return;
    }
}

void send_data(void) {
    float temperature_celsius = 0;
    uint8_t data[8] = {0}; // Full 8-byte array to match tx_msg.data_length_code
    twai_message_t tx_msg;
    tx_msg.identifier = CAN_FLC_TX_GET_STATUS;

    // Read temperature in Celsius
    if (tmp1075_get_temp_celsius(&TANWA_hardware.tmp1075[1], &temperature_celsius) != ESP_OK) {
        ESP_LOGE(TAG, "TEMP READ CELSIUS FAIL");
        return;
    }

    // Insert the float value into the data array starting at index 4
    insert_float_into_uint8_array(&temperature_celsius, data, 4);

    // Calculate the length of meaningful data
    tx_msg.data_length_code = 8; // Full length

    // Copy the data array into the tx_msg.data
    memcpy(tx_msg.data, data, tx_msg.data_length_code);

    // Transmit the message
    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) != ESP_OK) {
        ESP_LOGE(TAG, "TRANSMIT TEMP CELSIUS FAIL");
        return;
    }
}


void can_decode_message(twai_message_t rx_msg)
{
    switch(rx_msg.identifier)
    {
        case CAN_FLC_TX_GET_STATUS: 
                {
                ESP_LOGI(TAG, "FLC SEND STATUS");
                send_status();
                }break;
            
        case CAN_FLC_TX_GET_DATA:
                {
                ESP_LOGI(TAG , "FLC GET STATUS");
                send_data();
                }break;
        case CAN_FLC_TX_SOFT_RESET:
                {
                    can_flc_commands_t cmd = CAN_FLC_TX_SOFT_RESET;
                if(xQueueSend(CMDS_queue, &cmd, 0) != pdTRUE) 
                    {
                        ESP_LOGE(TAG, "TRANSMIT  CAN_FLC_TX_SOFT_RESET TO COMMANDS QUEUE FAIL");
                    }
                }break;
           
        case CAN_FLC_TX_NOTHING:
        {
             __asm__ __volatile__("nop");
        }break;

        default:
        break;
    
    }
}

void can_task(void *pvParameters)
{
 TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true) {
        twai_message_t rx_msg;
        if(twai_receive(&rx_msg, pdMS_TO_TICKS(100)) == ESP_OK) {
            can_decode_message(rx_msg);
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CAN_TASK_DEFAULT_FREQ));
    }
    vTaskDelete(NULL);
}

void run_can_task(void) {
    
        xTaskCreatePinnedToCore(can_task, "can_task", CAN_TASK_STACK_SIZE, NULL, CAN_TASK_PRIORITY,
                                &can_task_handle, CAN_TASK_CORE);
    }


