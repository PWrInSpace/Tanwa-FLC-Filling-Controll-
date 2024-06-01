
#include "functions.h"
#include "now.h"

volatile ModuleData moduleData;
Functions fcn;
uint32_t loopTimer;

void setup() {

    delay(100);
    Serial.begin(115200);
    if (moduleData.flashSave)   fcn.flashLogger.begin();
    if (moduleData.sdSave)      fcn.sdLogger.begin();

    fcn.initPeripherals();

    nowInit();
    nowAddPeer(adressObc, 0);

    loopTimer = SLEEP_TIME_MS;
}

void loop() {

    if (millis() - loopTimer >= moduleData.stateTimes[moduleData.obcState]) {

        loopTimer = millis();
        fcn.measure();
        esp_now_send(adressObc, (uint8_t *) &moduleData.dataToObc, sizeof(moduleData.dataToObc));
        
        // Push to flash and / or SD queues:
        if (moduleData.flashSave && fcn.stateInWakenUp()) fcn.flashLogger.push();
        if (moduleData.sdSave && fcn.stateInWakenUp()) fcn.sdLogger.push();
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);
    // Check things related to sleep mode and eventually go to sleep:
    fcn.checkSleepThings();
}
