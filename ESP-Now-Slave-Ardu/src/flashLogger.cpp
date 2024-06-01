#include "flashLogger.h"

// For use of static attribute:
QueueHandle_t FlashLogger::flashQueue = {};

void FlashLogger::begin() {

    LITTLEFS.begin(true);
    flashQueue = xQueueCreate(QUE_SIZE, sizeof(DataToSave));
    xTaskCreate((TaskFunction_t) logTask, "Flash Task", 16384, NULL, 1, NULL);

    // Check if call read:
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
    Serial.println("Press the boot button to read flash...");
    delay(500);
    if (!digitalRead(BOOT_BUTTON_PIN)) read();
    Serial.println("Flash reading skipped / finished.");
}

/**********************************************************************************************/

void FlashLogger::push() {

    xQueueSend(flashQueue, (uint8_t*) &moduleData.dataToSave, portMAX_DELAY);
}

/**********************************************************************************************/

void FlashLogger::read() {

    File file = LITTLEFS.open("/FlightData", "r");

    DataToSave data;
    while (file.readBytes((char*) &data, sizeof(data))) {
        Serial.println(data.toString());
    }

    file.close();
}

/**********************************************************************************************/

void FlashLogger::logTask() {

    File file;
    bool appendFlash = false;

    while (1) {

        if (uxQueueMessagesWaiting(flashQueue) > 0) {

            if (!appendFlash) {
                appendFlash = true;
                file = LITTLEFS.open("/FlightData", "w");
            }
            else {
                file = LITTLEFS.open("/FlightData", "a");
            }

            while (uxQueueMessagesWaiting(flashQueue)) {

                DataToSave tempData;
                xQueueReceive(flashQueue, &tempData, portMAX_DELAY);
                file.write((uint8_t*) &tempData, sizeof(tempData));
            }

            file.close();
        }

        vTaskDelay(3 / portTICK_PERIOD_MS);
    }
}