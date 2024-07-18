///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 25.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "app_init_task.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "TANWA_FLC_config.h"
#include "can_task.h"
//#include "mcu_adc_config.h"
#include "mcu_twai_config.h"


#define TAG "APP_INIT_TASK"

// static sdmmc_card_t sdmmc_card;

// static sd_card_t sd_card = SD_CARD_DEFAULT_CONFIG(sdmmc_card);

// static sd_card_config_t sd_card_conf = SD_CARD_CONFIG_DEFAULT_CONFIG();

#define APP_INIT_TASK_STACK_SIZE 4096
#define APP_INIT_TASK_PRIORITY 1
#define APP_INIT_TASK_CORE 0

extern TANWA_hardware_t TANWA_hardware;


void run_app_init(void) 
{
  xTaskCreatePinnedToCore(app_init_task, "app_init_task", APP_INIT_TASK_STACK_SIZE, NULL, APP_INIT_TASK_PRIORITY, NULL, APP_INIT_TASK_CORE);
}

void app_init_task(void* pvParameters) 
{
  ESP_LOGI(TAG, "### App initialization task started ###");
  esp_err_t ret = ESP_OK;

 ESP_LOGI(TAG, "Initializing MCU configuration...");

  ret |= TANWA_mcu_config_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "MCU configuration failed");
  } else {
    ESP_LOGI(TAG, "### MCU configuration success ###");
  }

  ESP_LOGI(TAG, "Initializing hardware...");
  
  ret |= TANWA_hardware_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Hardware initialization failed");
  } else {
    ESP_LOGI(TAG, "### Hardware initialization success ###");
  }

  ESP_LOGI(TAG, "### App initialization finished ###");
  

    ret |= mcu_twai_init();

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "CAN initialization failed");
    }else {
    ESP_LOGI(TAG, "### CAN initialization success ###");
  }

  run_can_task();
  vTaskDelay(pdMS_TO_TICKS(1000));
  //measure_task();
  vTaskDelete(NULL);
  
}