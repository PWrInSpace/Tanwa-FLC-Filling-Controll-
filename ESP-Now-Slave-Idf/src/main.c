#include "functions.h"
#include "now.h"

#define TAG "MAIN"

#ifdef TEMPLATE_CMD
    #include "cmd.h"
#endif

#ifdef TEMPLATE_FLASH
    #include "template_lib/flash_task.h"
#endif

#ifdef TEMPLATE_SD
    #include "template_lib/sd_task.h"
#endif

volatile ModuleData moduleData;
uint32_t loopTimer;

void app_main() {

    vTaskDelay(100 / portTICK_PERIOD_MS);
    initDataStructsAndUart();

    initPeripherals();

    nowInit();
    nowAddPeer(adressObc, 0);

#ifdef TEMPLATE_FLASH
    uint32_t flash_timer = getTimeMS();
    initFlashTask();
#endif

#ifdef TEMPLATE_SD
    uint32_t sd_timer = getTimeMS();
    initializeSPI();
    initSdTask();
#endif

#ifdef TEMPLATE_CMD
    if (CMD_initialize() != ESP_OK) {
        ESP_LOGE(TAG, "Unable to initialize commands");
    }
#endif

    loopTimer = SLEEP_TIME_MS;

    // Main loop:
    while (1) {
        if ((getTimeMS()) - loopTimer >= moduleData.stateTimes[moduleData.obcState]) {
            // This statement above works good only if both getTimeMS() and timer are unsigned (uint32_t).

            loopTimer = getTimeMS();
            measure();
            esp_now_send(adressObc, (uint8_t *) &moduleData.dataToObc, sizeof(moduleData.dataToObc));
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
        // Check things related to sleep mode and eventually go to sleep:
        checkSleepThings();

#ifdef TEMPLATE_FLASH
        if ((getTimeMS()) - flash_timer >= DATA_LOG_PERIOD) {
            flash_timer = getTimeMS();
            moduleData.dataToSave.uptime = flash_timer;
            sendDataToFlashTask((DataToSave*)&moduleData.dataToSave);
        }
#endif
#ifdef TEMPLATE_SD
        if ((getTimeMS()) - sd_timer >= DATA_LOG_PERIOD) {
            sd_timer = getTimeMS();
            moduleData.dataToSave.uptime = sd_timer;
            sendDataToSdTask((DataToSave*)&moduleData.dataToSave);
        }
#endif
    }
}