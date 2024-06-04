///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 30.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the structures of data for the slave TANWA submodules.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_TANWA_SLAVE_STRUCTS_H_
#define PWRINSPACE_TANWA_SLAVE_STRUCTS_H_

#include <stdbool.h>
#include <stdint.h>

///===-----------------------------------------------------------------------------------------===//
/// FLC - Filling Controller
///===-----------------------------------------------------------------------------------------===//

typedef struct {
    uint16_t status;
    uint8_t request;
    int16_t temperature;
} can_flc_status_t;

typedef struct {
    int16_t temperature_1;
    int16_t temperature_2;
    int16_t temperature_3;
    int16_t temperature_4;
} can_flc_data_t;

#endif // PWRINSPACE_SLAVE_STRUCTS_H_