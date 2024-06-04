#include "init_task.h"

#include "esp_log.h"
#include "esp_now_config.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "flash_task_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_expander.h"
#include "gps_task_config.h"
#include "i2c.h"
#include "lora.pb-c.h"
#include "lora_hw_config.h"
#include "lora_task.h"
#include "lora_task_config.h"
#include "mission_timer_config.h"
#include "processing_task_config.h"
#include "recovery_task_config.h"
#include "rocket_data.h"
#include "sd_card_config.h"
#include "settings_mem.h"
#include "spi.h"
#include "state_machine_config.h"
#include "system_timer_config.h"
#include "uart.h"
#include "utils.h"
#include "vbat_wrapper.h"

#define TAG "INIT"

inline static void CHECK_RESULT_ESP(esp_err_t res, char *message) {
    if (res == ESP_OK) {
        ESP_LOGI(TAG, "Initialized %s", message);
        return;
    }
    ESP_LOGE(TAG, "Init error %s", message);
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(res));
    esp_restart();
}

inline static void CHECK_RESULT_BOOL(esp_err_t res, char *message) {
    if (res == true) {
        ESP_LOGI(TAG, "Initialized %s", message);
        return;
    }
    ESP_LOGE(TAG, "Init error %s", message);
    esp_restart();
}

static void TASK_init(void *arg) {
    CHECK_RESULT_ESP(settings_init(), "Change state");
    vTaskDelete(NULL);
}

void run_init_task(void) {
    xTaskCreatePinnedToCore(TASK_init, "Init", 8800, NULL, 20, NULL, PRO_CPU_NUM);
}