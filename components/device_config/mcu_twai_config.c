// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "mcu_twai_config.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TAG "MCU_TWAI"

#define TX_GPIO_NUM                     28 // TODO: CHECK PINOUT
#define RX_GPIO_NUM                     27 // TODO: CHECK PINOUT

mcu_twai_config_t mcu_twai_config = {
    .tx_gpio_num = 17,
    .rx_gpio_num = 16,
    .mode = TWAI_MODE_NORMAL,
    .g_config = {
        .mode = TWAI_MODE_NORMAL,
        .tx_io = 17,
        .rx_io = 16,
        .clkout_io = TWAI_IO_UNUSED,
        .bus_off_io = TWAI_IO_UNUSED,
        .tx_queue_len = 1,
        .rx_queue_len = 5,
        .alerts_enabled = TWAI_ALERT_NONE, // for now - ToDo: change and test alerts
        .clkout_divider = 0,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    },
    .t_config = TWAI_TIMING_CONFIG_25KBITS(),
    .f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL(),
};


esp_err_t mcu_twai_init() {
    esp_err_t err;
    err = twai_driver_install(&(mcu_twai_config.g_config), 
                              &(mcu_twai_config.t_config),
                              &(mcu_twai_config.f_config));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "TWAI driver install error");
      return err;
    }
    return ESP_OK;
}

