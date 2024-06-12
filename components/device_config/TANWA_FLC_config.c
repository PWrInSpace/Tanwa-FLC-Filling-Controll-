///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "TANWA_FLC_config.h"

#include "esp_log.h"
//#include "mcu_gpio_config.h"
#include "mcu_i2c_config.h"
#include "mcu_spi_config.h"
#include "pinout.h"

#define TAG "TANWA_CONFIG"

#define IOEXP_MODE (IOCON_INTCC | IOCON_INTPOL | IOCON_ODR | IOCON_MIRROR)

#define TMP1075_QUANTITY 2

#define MAX31856_QUANTITY 2

TANWA_hardware_t TANWA_hardware = 
{
    .tmp1075 = {
        {
            ._i2c_write = _mcu_i2c_write,
            ._i2c_read = _mcu_i2c_read,
            .i2c_address = CONFIG_I2C_TMP1075_TS1_ADDR,
            .config_register = 0,
        },
        {
            ._i2c_write = _mcu_i2c_write,
            ._i2c_read = _mcu_i2c_read,
            .i2c_address = CONFIG_I2C_TMP1075_TS2_ADDR,
            .config_register = 0,
        },
    },
}


esp_err_t TANWA_mcu_config_init()
 {
    esp_err_t ret = ESP_OK;
    ret |= mcu_gpio_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize GPIO");
        return ret;
    } else {
        ESP_LOGI(TAG, "GPIO initialized");
    }
    
    ret |= mcu_i2c_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C");
        return ret;
    } else {
        ESP_LOGI(TAG, "I2C initialized");
    }
    ret |= mcu_spi_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI");
        return ret;
    } else {
        ESP_LOGI(TAG, "SPI initialized");
    }
    ret |= mcu_twai_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize TWAI");
        return ret;
    } else {
        ESP_LOGI(TAG, "TWAI initialized");
    }
    return ESP_OK;
}

esp_err_t TANWA_hardware_init() 
{

    //INIT TEMP SENS TMP1075
    uint8_t ret = 0;
    for (int i=0; i<TMP1075_QUANTITY; i++)
    {
    ret = tmp1075_init(&(TANWA_hardware.tmp1075[i]));
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "Failed to initialize TMP1075 sensor %d", i+1);
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "TMP1075 sensor 1 initialized");
    }
    }
    for (int i=0; i<MAX31856_QUANTITY; i++)
    {

    }

    //INIT THERMOCOUPLES
    uint8_t fault_val;
    max31856_cfg thermocouple_1;
    max31856_cfg thermocouple_2;

  ESP_LOGI(TAG, "Thermocouple initialization...");
  max31856_init(&thermocouple[0], THERMOCOUPLE_CS1); 
  max31856_init(&thermocouple[1], THERMOCOUPLE_CS2);
  ESP_LOGI(TAG, "Thermocouple set type...");
  thermocouple_set_type(&thermocouple[1], MAX31856_TCTYPE_K);
  thermocouple_set_type(&thermocouple[2], MAX31856_TCTYPE_K);
  ESP_LOGI(TAG, "Thermocouple read fault...");
  fault_val = thermocouple_read_fault(&thermocouple_1, true);
  if(fault_val==1)
  {
    return ESP_FAIL;
  }
  fault_val = thermocouple_read_fault(&thermocouple_2, true);
  if(fault_val==1)
  {
    return ESP_FAIL;
  }

    return ESP_OK;
}