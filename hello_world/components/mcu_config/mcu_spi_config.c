///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "mcu_spi_config.h"

#include "mcu_gpio_config.h"

#include "esp_log.h"

#define TAG "MCU_SPI"

static mcu_spi_config_t spi_config = MCU_SPI_DEFAULT_CONFIG();

esp_err_t mcu_spi_init(void) {
    esp_err_t ret = ESP_OK;
    if (spi_config.spi_init_flag) {
      return ESP_OK;
    }

    ret = spi_bus_initialize(spi_config.host_id, &spi_config.bus_config,
                            SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    ret = spi_bus_add_device(spi_config.host_id, &spi_config.dev_config,
                            &spi_config.spi_handle);
    ESP_ERROR_CHECK(ret);
    spi_config.spi_init_flag = true;
    return ret;
}

esp_err_t mcu_spi_deinit(void) {
    esp_err_t ret = ESP_OK;
    if (!spi_config.spi_init_flag) {
      return ESP_OK;
    }
    ret = spi_bus_remove_device(spi_config.spi_handle);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_free(spi_config.host_id);
    ESP_ERROR_CHECK(ret);
    spi_config.spi_init_flag = false;
    return ret;
}

bool _lora_SPI_transmit(uint8_t _in[2], uint8_t _out[2]) {
    spi_transaction_t t = {.flags = 0,
                          .length = 8 * sizeof(uint8_t) * 2,
                          .tx_buffer = _out,
                          .rx_buffer = _in};

    _mcu_gpio_set_level(LORA_CS_GPIO_INDEX, 0);
    // TODO(Gliwus): implement thread safe SPI (also with SD card and flash)
    spi_device_transmit(spi_config.spi_handle, &t);
    _mcu_gpio_set_level(LORA_CS_GPIO_INDEX, 1);
    return true;
}