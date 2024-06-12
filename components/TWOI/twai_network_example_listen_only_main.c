/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/*
 * The following example demonstrates a Listen Only node in a TWAI network. The
 * Listen Only node will not take part in any TWAI bus activity (no acknowledgments
 * and no error frames). This example will execute multiple iterations, with each
 * iteration the Listen Only node will do the following:
 * 1) Listen for ping and ping response
 * 2) Listen for start command
 * 3) Listen for data messages
 * 4) Listen for stop and stop response
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"
#include "can_commands.h"
/* --------------------- Definitions and static variables ------------------ */
//Example Configuration
#define NO_OF_ITERS                     3
#define RX_TASK_PRIO                    9
#define TX_GPIO_NUM                     28 // TODO: CHECK PINOUT
#define RX_GPIO_NUM                     27 // TODO: CHECK PINOUT
#define EXAMPLE_TAG                     "TWAI Listen Only"
#define STACK_SIZE                      4096

#ifdef master_conf

#endif

#define GET_DATA                        0x0D1
#define GET_STATUS                      0x0D0
#define SOFT_RESTART                    0x0D9
#define NOTHING                         0x0DF

static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
//Set TX queue length to 0 due to listen only mode
static const twai_general_config_t g_config = {.mode = TWAI_MODE_NORMAL,
                                               .tx_io = TX_GPIO_NUM, .rx_io = RX_GPIO_NUM,
                                               .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED,
                                               .tx_queue_len = 5, .rx_queue_len = 5,
                                               .alerts_enabled = TWAI_ALERT_NONE,
                                               .clkout_divider = 0
                                              };

static SemaphoreHandle_t rx_sem;

static twai_message_t temp_mess = {
    .identifier = 0x01B, // TODO: CHECK IDENTIFIER
    .data_length_code = 8,
    .data = {0, 0, 0, 0, 0, 0, 0, 0},
};

static float parse_float_from_uint8_array(uint8_t *bytes, uint8_t start_idx) {
    if (start_idx > 4) {
        ESP_LOGE(EXAMPLE_TAG, "Invalid start index");
        return 0.0f;
    }
    float value;
    memcpy(&value, bytes + start_idx, sizeof(float));
    return value;
}

void insert_float_into_uint8_array(float *value, uint8_t *bytes, uint8_t start_idx) {
    if (start_idx > 4) { 
        ESP_LOGE(EXAMPLE_TAG, "Invalid start index");
        return;
    }
    memcpy(bytes + start_idx, value, sizeof(float));
}

/* --------------------------- Tasks and Functions -------------------------- */

static void twai_receive_task(void *arg)
{
    
}

//TODO: COPY FUNCTIONS TO CAN_LISTEN_COFNIGURE().C AND CAN_LISTEN_TASK().C
void twai_configure_listen()
{
   
}





void create_task()
{
    float temp = 69.9f; 
    uint32_t temp_raw = 12345;
    memcpy(temp_mess.data, &temp, sizeof(float));
    memcpy(temp_mess.data + 4, &temp_raw, sizeof(uint32_t));

    ESP_LOGI(EXAMPLE_TAG, "TWAI message data: [%d, %d, %d, %d, %d, %d, %d, %d]",
             temp_mess.data[0], temp_mess.data[1], temp_mess.data[2], temp_mess.data[3],
             temp_mess.data[4], temp_mess.data[5], temp_mess.data[6], temp_mess.data[7]);

    rx_sem = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(twai_receive_task, "TWAI_rx", STACK_SIZE, NULL, RX_TASK_PRIO, NULL, tskNO_AFFINITY);
    
}
