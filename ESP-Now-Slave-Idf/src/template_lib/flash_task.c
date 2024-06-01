#include "esp_log.h"
#include "template_lib/flash.h"
#include "functions.h"

#define TAG "MEM"

typedef struct {
    bool formated;
    uint32_t wrote_size;
    uint32_t max_size;
    FILE* file;
} mem_flash_t;

static struct {
    QueueHandle_t queue;
    bool flash_formated;
} mem = {
    .queue = NULL,
    .flash_formated = false,
};


static void flash_mem_init(mem_flash_t *fl) {
    terminate_task_on_error(FLASH_init(1) != FLASH_OK, "FLASH INIT");
    // sometimes above 2/3 used memory, writing to flash take more than 150ms
    fl->max_size = FLASH_get_total_size() * 3 / 5;
    fl->wrote_size = 0;
}

static void flash_mem_open(mem_flash_t *fl) {
    FLASH_format();

    fl->file = fopen(FLASH_CREATE_PATH(FLASH_FILE_NAME), "a");
    terminate_task_on_error(fl->file == NULL, "File open");
}

static void flash_mem_write(mem_flash_t *fl, DataToSave *data, size_t size) {
    if (fl->max_size <= fl->wrote_size) {
        ESP_LOGW(TAG, "MAX SIZE");
        return;
    }

    size_t ret = fwrite(data, size, 1, fl->file);
    if (ret != 1) {
        ESP_LOGE(TAG, "Flash write error");
    } else {
        fl->wrote_size += size;
    }
}

static void flashTask(void *arg) {
    ESP_LOGI(TAG, "RUNNING FLASH TASK");
    DataToSave data;
    mem_flash_t m_flash;
    flash_mem_init(&m_flash);

    memory_check_start_condition(); // Blocking till required state.

    flash_mem_open(&m_flash); // Fotmating flash.
    mem.flash_formated = true;

    while(1) {
        if (uxQueueMessagesWaiting(mem.queue) > DATA_ONE_SHOT_SAVE_NB) {
            while (uxQueueMessagesWaiting(mem.queue) > 0) {
                xQueueReceive(mem.queue, &data, portMAX_DELAY);
                flash_mem_write(&m_flash, &data, sizeof(data));
            }
            fflush(m_flash.file);
        }

        memory_check_end_condition();

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}


bool initFlashTask(void) {
    mem.queue = xQueueCreate(DATA_QUEUE_SIZE, sizeof(DataToSave));
    if (mem.queue == NULL) {
        return false;
    }

    xTaskCreatePinnedToCore(flashTask, "flash task", 8000, NULL, 5, NULL, APP_CPU_NUM);
    return true;
}

bool sendDataToFlashTask(DataToSave *data) {
    if (can_write_to_flash() == false) {
        return false;
    }

    if (mem.flash_formated == false) {
        return false;
    }

    if (mem.queue == NULL) {
        return false;
    }

    if (xQueueSend(mem.queue, data, 10) == pdFALSE) {
        ESP_LOGW(TAG, "Unable to add data to flash queue");
        return false;
    }

    return true;
}
