///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "ads1115.h"
#include "esp_err.h"
#include "pressure_driver.h"
#include "tmp1075.h"
#include "max31856.h"

#define TMP1075_QUANTITY 1
#define MAX31856_QUANTITY 2
// TANWA hardware
typedef struct {

    tmp1075_struct_t tmp1075[TMP1075_QUANTITY];
    max31856_cfg thermocouple[MAX31856_QUANTITY];
    ads1115_struct_t ads1115;

} TANWA_hardware_t;

typedef struct {
    pressure_driver_struct_t pressure_driver;
} TANWA_utility_t;

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

esp_err_t TANWA_mcu_config_init();

esp_err_t TANWA_hardware_init();
