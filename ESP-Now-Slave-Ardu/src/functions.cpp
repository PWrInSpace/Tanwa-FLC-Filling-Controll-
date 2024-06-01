#include "functions.h"

// IMPORTANT! To implementation of each module:

void Functions::initPeripherals() {

    #warning "Function is incomplete"
}

/**********************************************************************************************/

// To implementation of each module:

void Functions::measure() {

    #warning "Function is incomplete"
}

/**********************************************************************************************/

// To implementation of each module:

void Functions::rxNowHandler(const uint8_t *incomingData, int len) {

    DataFromObc rxData;
    if (len == sizeof(rxData)) {

        memcpy((uint8_t*) &rxData, incomingData, len);
        // From this moment you can use rxData.commandNum and rxData.commandArg.

        #warning "Function is incomplete"
    }
}

// IMPORTANT END!

/**********************************************************************************************/
/**********************************************************************************************/
/**********************************************************************************************/

uint32_t Functions::getPowerTime_ms() {

    timeval tv;
    timezone tz;

    gettimeofday(&tv, (void*) &tz);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

/**********************************************************************************************/

void Functions::goToSleep() {

    Serial.println("Going to sleep!"); // DEBUG
    esp_sleep_enable_timer_wakeup((SLEEP_TIME_MS) * 10e2);
    esp_deep_sleep_start();
}

/**********************************************************************************************/

bool Functions::stateInWakenUp() {

    return moduleData.stateTimes[moduleData.obcState] != SLEEP_TIME_MS;
}

/**********************************************************************************************/

void Functions::checkSleepThings() {

    if (!stateInWakenUp()) {
        Serial.println("Waiting for OBC state..."); // DEBUG
        vTaskDelay(ANSWER_WAIT_MS / portTICK_PERIOD_MS);

        if (!stateInWakenUp()) {
            esp_now_send(adressObc, (uint8_t *) &moduleData.dataToObc, sizeof(moduleData.dataToObc));
            goToSleep();
        }
    }
}