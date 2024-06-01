#include "esp_log.h"
#define TAG "MEM"

#include "template_lib/sdcard.h"
#include "functions.h"

typedef struct {
    char data_buffer[SD_BUFFER_SIZE];
    char path[PATH_FLIE_SIZE(SD_FILE_NAME)];
    FILE* file;
} mem_sd_t;

static struct {
    QueueHandle_t queue;
    bool running;
} mem = {
    .queue = NULL,
    .running = false,
};

static void sd_mem_init(sd_card_t *sd_card, mem_sd_t *sd) {
    bool ret = SD_init(sd_card, SD_SPI_HOST, SD_CS_PIN, SDCARD_MOUNT_POINT);
    ESP_LOGE(TAG, "DUPAAAA");
    terminate_task_on_error(ret == false, "SD init error");

    if (create_path_to_file(sd->path, sizeof(sd->path)) == false) {
        ESP_LOGW(TAG, "CAN'T CREATE FILE PATH, USING -> %s", sd->path);
    } else {
        ESP_LOGI(TAG, "Using file path %s", sd->path);
    }

    SD_write(sd_card, sd->path, SD_HEADER, sizeof(SD_HEADER));
}

static void sd_mem_write(sd_card_t *sd_card, mem_sd_t *sd, DataToSave *data) {
    if (sd_card->mounted == false) {
        if (SD_mount(sd_card) == false) {
            return;
        }
    }

    if (SD_is_ok(sd_card) == false) {
        SD_remount(sd_card);
        return;
    }

    if (sd->file == NULL) {
        return;
    }

    if (create_sd_string(data, sd->data_buffer, sizeof(sd->data_buffer)) == false) {
        ESP_LOGE(TAG, "Create sd string errror :C");
        return;
    }

    fprintf(sd->file, sd->data_buffer, sd_card->card->cid.name);
}



static void sdTask(void *args) {
    ESP_LOGI(TAG, "RUNNING SD TASK");
    DataToSave data;

    mem_sd_t m_sd = {.path = SD_CREATE_FILE_PREFIX(SD_FILE_NAME)};
    sd_card_t sd_card;

    memory_check_start_condition();

    sd_mem_init(&sd_card, &m_sd);
    mem.running = true;

    while(1) {
        if (uxQueueMessagesWaiting(mem.queue) > DATA_ONE_SHOT_SAVE_NB) {
            m_sd.file = fopen(m_sd.path, "a");
            while (uxQueueMessagesWaiting(mem.queue) > 0) {
                xQueueReceive(mem.queue, &data, portMAX_DELAY);
                sd_mem_write(&sd_card, &m_sd, &data);

            }
            fclose(m_sd.file);
        }
        memory_check_end_condition();

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}



bool initSdTask(void) {
    // check_end_condition();
    mem.queue = xQueueCreate(DATA_QUEUE_SIZE, sizeof(DataToSave));
    if (mem.queue == NULL) {
        return false;
    }

    xTaskCreatePinnedToCore(sdTask, "sd task", 8000, NULL, 5, NULL, APP_CPU_NUM);

    return true;
}

bool sendDataToSdTask(DataToSave *data) {
    if (can_write_to_sd() == false) {
        return false;
    }

    if (mem.running == false) {
        return false;
    }

    if (mem.queue == NULL) {
        return false;
    }

    if (xQueueSend(mem.queue, data, 10) == pdFALSE) {
        ESP_LOGW(TAG, "Unable to add data to sd mem.queue");
        return false;
    }

    return true;
}
