///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#include "tmp1075.h"
#include "max31856.h"
// TANWA hardware
typedef struct {
    tmp1075_struct_t tmp1075[2];
    max31856_cfg thermocouple[2];
} TANWA_hardware_t;

extern TANWA_hardware_t TANWA_hardware;

esp_err_t TANWA_mcu_config_init();

esp_err_t TANWA_hardware_init();
