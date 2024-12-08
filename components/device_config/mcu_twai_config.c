// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "mcu_twai_config.h"
#include <inttypes.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <inttypes.h>

#define TAG "MCU_TWAI"

#define TX_GPIO_NUM 17
#define RX_GPIO_NUM 16
#define ERROR_THRESHOLD 255

mcu_twai_config_t mcu_twai_config = 
{
    .tx_gpio_num = 17,
    .rx_gpio_num = 16,
    .mode = TWAI_MODE_NORMAL,
    .g_config = {
        .mode = TWAI_MODE_NORMAL,
        .tx_io = 17,
        .rx_io = 16,
        .clkout_io = TWAI_IO_UNUSED,
        .bus_off_io = TWAI_IO_UNUSED,
        .tx_queue_len = 10,
        .rx_queue_len = 10,
        .alerts_enabled = TWAI_ALERT_ALL ,
        .clkout_divider = 0,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    },
    .t_config = TWAI_TIMING_CONFIG_250KBITS(),
    .f_config = {
        .acceptance_code = 0x0D0,
        .acceptance_mask = 0x00F,
        .single_filter = true,
    },
};


void handle_alert(const char *alert_message) {
    ESP_LOGI(TAG, "Alert triggered: %s", alert_message);
    twai_stop();
    twai_start();
}

void monitor_twai_status(void) {
    twai_status_info_t status;
    esp_err_t err = twai_get_status_info(&status);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get TWAI status info: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "TX Error Count: %" PRIu32 ", RX Error Count: %" PRIu32, status.tx_error_counter, status.rx_error_counter);

    if (status.tx_error_counter > ERROR_THRESHOLD) {
        ESP_LOGW(TAG, "TX Error Counter exceeded threshold! Count: %" PRIu32, status.tx_error_counter);
        handle_alert("TX Error Threshold Exceeded");
    }

    if (status.rx_error_counter > ERROR_THRESHOLD) {
        ESP_LOGW(TAG, "RX Error Counter exceeded threshold! Count: %" PRIu32, status.rx_error_counter);
        handle_alert("RX Error Threshold Exceeded");
    }
    
    if (status.state == TWAI_STATE_BUS_OFF) {
        ESP_LOGW(TAG, "Bus Off condition detected");
        handle_alert("Bus Off detected");
        err = twai_initiate_recovery();
        if (err == ESP_OK) {
            ESP_LOGI("TWAI", "TWAI recovery initiated successfully");
        } else {
            ESP_LOGE("TWAI", "Failed to initiate TWAI recovery: %s", esp_err_to_name(err));
        }
    }
}




esp_err_t mcu_twai_init()
{
    esp_err_t err;
    err = twai_driver_install(&(mcu_twai_config.g_config),
                              &(mcu_twai_config.t_config),
                              &(mcu_twai_config.f_config));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "TWAI driver install error");
        return err;
    }
    return ESP_OK;
}
