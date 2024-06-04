#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

#include <esp_log.h>
#include "max31856.h"

#define THERMOCOUPLE_CS1 27U //zmiana
#define THERMOCOUPLE_CS2 32U //zmiana


esp_err_t hardware_init()
{
    //INIT THERMOCOUPLES
    uint8_t fault_val;
    max31856_cfg thermocouple_1;
    max31856_cfg thermocouple_2;

  ESP_LOGI(TAG, "Thermocouple initialization...");
  max31856_init(&thermocouple_1, THERMOCOUPLE_CS1); 
  max31856_init(&thermocouple_2, THERMOCOUPLE_CS2);
  ESP_LOGI(TAG, "Thermocouple set type...");
  thermocouple_set_type(&thermocouple_1, MAX31856_TCTYPE_K);
  thermocouple_set_type(&thermocouple_2, MAX31856_TCTYPE_K);
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
    //INIT TEMP SENS
   uint8_t ret = 0;
    ret = tmp1075_init(&(TANWA_hardware.tmp1075[0]));
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "Failed to initialize TMP1075 sensor 1");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "TMP1075 sensor 1 initialized");
    }
    ret = tmp1075_init(&(TANWA_hardware.tmp1075[1]));
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "Failed to initialize TMP1075 sensor 2");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "TMP1075 sensor 2 initialized");
    }
   return ESP_OK;
}