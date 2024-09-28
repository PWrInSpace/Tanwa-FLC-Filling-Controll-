///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 20.06.2024 by Mateusz Kłosiński
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
#include "data_handling.h"

#define TAG1 "APP_INIT_TASK"

#define APP_INIT_TASK_STACK_SIZE 4096
#define APP_INIT_TASK_PRIORITY 1
#define APP_INIT_TASK_CORE 0

extern TANWA_hardware_t TANWA_hardware;

void run_app_init(void)
{
  xTaskCreatePinnedToCore(app_init_task, "app_init_task", APP_INIT_TASK_STACK_SIZE, NULL, APP_INIT_TASK_PRIORITY, NULL, APP_INIT_TASK_CORE);
}

void app_init_task(void *pvParameters)
{
  ESP_LOGI(TAG1, "### App initialization task started ###");
  esp_err_t ret = ESP_OK;

  ESP_LOGI(TAG1, "Initializing MCU configuration...");

  ret = TANWA_mcu_config_init();
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG1, "MCU configuration failed");
  }
  else
  {
    ESP_LOGI(TAG1, "### MCU configuration success ###");
  }

  ESP_LOGI(TAG1, "Initializing hardware...");
  vTaskDelay(pdMS_TO_TICKS(1000));
  ret = TANWA_hardware_init();
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG1, "Hardware initialization failed");
  }
  else
  {
    ESP_LOGI(TAG1, "### Hardware initialization success ###");
  }
  ret = rtos_util_init();
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG1, "RTOS util initialization failed");
  }
  else
  {
    ESP_LOGI(TAG1, "### RTOS util initialization success ###");
  }

  run_can_task();

  vTaskDelay(pdMS_TO_TICKS(1000));
  run_data_handling_task();
  ESP_LOGI(TAG1, "### App initialization finished ###");
  vTaskDelete(NULL);
}