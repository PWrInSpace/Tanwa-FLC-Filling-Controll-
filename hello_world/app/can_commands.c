///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 01.06.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "can_commands.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "TANWA_data.h"

#define TAG "CAN_COMMANDS"


#define CAN_FLC_STATUS_POS 0
#define CAN_FLC_STATUS_REQUEST_POS 2
#define CAN_FLC_STATUS_TEMP_POS 6

#define CAN_FLC_DATA_TEMP_1_POS 0
#define CAN_FLC_DATA_TEMP_2_POS 2
#define CAN_FLC_DATA_TEMP_3_POS 4
#define CAN_FLC_DATA_TEMP_4_POS 6



void parse_can_flc_status(twai_message_t rx_message) {
    // update flc status
    can_flc_status_t flc_status = {
        .status = *((uint16_t*)rx_message.data + CAN_FLC_STATUS_POS),
        .request = *((uint8_t*)(rx_message.data + CAN_FLC_STATUS_REQUEST_POS)),
        .temperature = *((int16_t*)(rx_message.data + CAN_FLC_STATUS_TEMP_POS)),
    };
    ESP_LOGI(TAG, "FLC status: status: %d, request: %d, temperature: %d", flc_status.status, flc_status.request, flc_status.temperature);
    tanwa_data_update_can_flc_status(&flc_status);

}

void parse_can_flc_data(twai_message_t rx_message) {
    // update flc data
    can_flc_data_t flc_data = {
        .temperature_1 = *((int16_t*)rx_message.data + CAN_FLC_DATA_TEMP_1_POS),
        .temperature_2 = *((int16_t*)(rx_message.data + CAN_FLC_DATA_TEMP_2_POS)),
        .temperature_3 = *((int16_t*)(rx_message.data + CAN_FLC_DATA_TEMP_3_POS)),
        .temperature_4 = *((int16_t*)(rx_message.data + CAN_FLC_DATA_TEMP_4_POS)),
    };
    ESP_LOGI(TAG, "FLC data: temperature 1: %d, temperature 2: %d, temperature 3: %d, temperature 4: %d", flc_data.temperature_1, flc_data.temperature_2, flc_data.temperature_3, flc_data.temperature_4);
    tanwa_data_update_can_flc_data(&flc_data);
}