///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 31.05.2024 by Michał Kos
///
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

#define TAG "CAN_TASK"

#define CAN_TASK_STACK_SIZE 4096
#define CAN_TASK_PRIORITY 5
#define CAN_TASK_CORE 1
#define CAN_TASK_DEFAULT_FREQ 1000



#define NO_OF_ITERS                     3
#define RX_TASK_PRIO                    9
#define TX_GPIO_NUM                     28 // TODO: CHECK PINOUT
#define RX_GPIO_NUM                     27 // TODO: CHECK PINOUT
#define EXAMPLE_TAG                     "TWAI Listen Only"
#define STACK_SIZE                      4096

#ifdef master_conf

#endif

#define GET_DATA                        0x0D1
#define GET_STATUS                      0x0D0
#define SOFT_RESTART                    0x0D9
#define NOTHING                         0x0DF

static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
//Set TX queue length to 0 due to listen only mode
static const twai_general_config_t g_config = {.mode = TWAI_MODE_NORMAL,
                                               .tx_io = TX_GPIO_NUM, .rx_io = RX_GPIO_NUM,
                                               .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED,
                                               .tx_queue_len = 5, .rx_queue_len = 5,
                                               .alerts_enabled = TWAI_ALERT_NONE,
                                               .clkout_divider = 0
                                              };

static SemaphoreHandle_t rx_sem;

static twai_message_t temp_mess = {
    .identifier = 0x0D, // TODO: CHECK IDENTIFIER
    .data_length_code = 8,
    .data = {0, 0, 0, 0, 0, 0, 0, 0},
};
static TaskHandle_t can_task_handle = NULL;

void run_can_task(void) 
{

     //Install and start TWAI driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(EXAMPLE_TAG, "Driver installed");
    
    if (twai_start() != ESP_OK) 
    {
      ESP_LOGE(TAG, "TWAI start error");
    } 
    else 
    {
        ESP_LOGI(TAG, "Driver started");
        rx_sem = xSemaphoreCreateBinary();
        if(rx_sem==NULL)
        {
            ESP_LOGE(EXAMPLE_TAG, "Failed to create queue");
            return;
        }
        xTaskCreatePinnedToCore(can_task_recieve, "can_task_recieve", CAN_TASK_STACK_SIZE, NULL, CAN_TASK_PRIORITY,
                                &can_task_handle, CAN_TASK_CORE);

        xSemaphoreGive(rx_sem);                     //Start RX task
        vTaskDelay(pdMS_TO_TICKS(100));
        xSemaphoreTake(rx_sem, portMAX_DELAY);      //Wait for RX task to complete

    }
}



void can_task_recieve(void* pvParameters)
{
      
      //TODO: Semaphor recieve and parse data

    xSemaphoreTake(rx_sem, portMAX_DELAY);
    float temp; 
    temp= *((float*)temp_mess.data);
    //TODO: DO DODANIA STATUS
    
    //temp_raw = *((uint32_t*)(temp_mess.data + 4));

    while (1) 
    {
        twai_message_t rx_msg;
        if (twai_receive(&rx_msg, pdMS_TO_TICKS(100)) == ESP_OK) {
            if (rx_msg.identifier == CAN_FLC_TX_GET_STATUS) {
                ESP_LOGI(EXAMPLE_TAG, "FLC GET DATA");
                if (twai_transmit(&temp_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
                    ESP_LOGI(EXAMPLE_TAG, "temp sent: %f", temp);
                }
            } else if (rx_msg.identifier == CAN_FLC_TX_GET_DATA) {
                ESP_LOGI(EXAMPLE_TAG, "FLC GET STATUS");
                if (twai_transmit(&temp_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
                    ESP_LOGI(EXAMPLE_TAG, "temp sent: %f", temp);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);   

    }
}

void stop_can_task(void);
{
    //Stop and uninstall TWAI driver
    ESP_ERROR_CHECK(twai_stop());
    ESP_LOGI(EXAMPLE_TAG, "Driver stopped");
    ESP_ERROR_CHECK(twai_driver_uninstall());
    ESP_LOGI(EXAMPLE_TAG, "Driver uninstalled");

    //Cleanup
    vSemaphoreDelete(rx_sem);
}
