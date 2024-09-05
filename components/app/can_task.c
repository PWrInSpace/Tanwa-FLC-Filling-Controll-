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

void insert_int16_t_into_uint8_array(int16_t *value, uint8_t *bytes, uint8_t start_idx) 
{
    if (start_idx > 2) { 
        ESP_LOGE(TAG, "Invalid start index");
        return;
    }
    memcpy(bytes + start_idx, value, sizeof(int16_t));
}

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
    for(int i =0; i<TMP1075_QUANTITY; i++)
    {
    if (tmp1075_get_temp_celsius(&TANWA_hardware.tmp1075[i], &temperature_celsius[i]) != ESP_OK) 
    {
        ESP_LOGE(TAG, "TEMP READ CELSIUS FAIL");
        return;
    }
    }

    for(int i =0; i<TMP1075_QUANTITY; i++)
    {
        temp_parsed[i] = temperature_celsius[i];
    }

    // Insert the float value into the data array starting at index x
    insert_int16_t_into_uint8_array(&temp_parsed[0], data, 2);

    tx_msg.data_length_code = 8; // Full length

    // Copy the data array into the tx_msg.data
    memcpy(tx_msg.data, data, tx_msg.data_length_code);

    // Transmit the message
    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) != ESP_OK) {
        ESP_LOGE(TAG, "TRANSMIT TEMP CELSIUS FAIL");
        return;
    }
}

void send_data(void) 

{
    twai_message_t tx_msg;
    tx_msg.identifier = CAN_FLC_TX_DATA;
    tx_msg.data_length_code = 8;

    uint8_t data[8] = {0};
    int16_t temp[MAX31856_QUANTITY] = {0};
    int16_t temp_cj[MAX31856_QUANTITY] = {0};

    xQueueReceive(ThermoTemp_queue, &temp, 0);
    xQueueReceive(ThermoTemp_queue_cj, &temp_cj, 0);


int i=0;
while(i<MAX31856_QUANTITY)
{
    for (int j=0; j<6 ; j=+4)
{
    insert_int16_t_into_uint8_array(&temp[i], data, j);
    insert_int16_t_into_uint8_array(&temp_cj[i], data, j+2);
    i++;
    
}

if (i>MAX31856_QUANTITY-1) break;

}

    memcpy(tx_msg.data, data, tx_msg.data_length_code);

     // Transmit the message
    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) != ESP_OK) {
        ESP_LOGE(TAG, "TRANSMIT ThermoCouple FAIL");
        return;
    }
    
}

void send_data_pressure(void) 

{
    twai_message_t tx_msg;
    tx_msg.identifier = CAN_FLC_TX_DATA_PRESSURE;
    tx_msg.data_length_code = 8;

    uint8_t data[8] = {0};
    int16_t pressure[4] = {0};

    xQueueReceive(PressureSens, &pressure, 0);

    memcpy(pressure,data,4*sizeof(int16_t));

    memcpy(tx_msg.data, data, tx_msg.data_length_code);

     // Transmit the message
    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) != ESP_OK) {
        ESP_LOGE(TAG, "TRANSMIT ThermoCouple FAIL");
        return;
    }
    
}


void can_decode_message(twai_message_t rx_msg)
{
    switch(rx_msg.identifier)
    {
        case CAN_FLC_RX_GET_STATUS: 
                {
                ESP_LOGI(TAG, "FLC RX SEND_STATUS");
                send_status();
                }break;
            
        case CAN_FLC_RX_GET_DATA:
                {
                ESP_LOGI(TAG , "FLC RX GET_DATA");
                send_data();
                }break;

        case CAN_FLC_RX_GET_DATA_PRESSURE:
                {
                ESP_LOGI(TAG , "FLC RX GET_DATA");
                send_data_pressure();
                }break;
        case CAN_FLC_RX_SOFT_RESET:
                {
                    can_flc_commands_t cmd = CAN_FLC_RX_SOFT_RESET;
                if(xQueueSend(CMDS_queue, &cmd, 0) != pdTRUE) 
                    {
                        ESP_LOGE(TAG, "TRANSMIT  CAN_FLC_TX_SOFT_RESET TO COMMANDS QUEUE FAIL");
                    }
                }break;
           
        case CAN_FLC_RX_NOTHING:
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

void run_can_task(void) 
{
    
        xTaskCreatePinnedToCore(can_task, "can_task", CAN_TASK_STACK_SIZE, NULL, CAN_TASK_PRIORITY,
                                &can_task_handle, CAN_TASK_CORE);
}





 

