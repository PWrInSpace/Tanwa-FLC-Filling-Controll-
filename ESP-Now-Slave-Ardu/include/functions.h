#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Arduino.h"
#include <sys/time.h>
#include "dataStructs.h"
#include "now.h"
#include "flashLogger.h"
#include "sdLogger.h"

class Functions {

    // Not to touch:
    uint32_t getPowerTime_ms();

public:

    FlashLogger flashLogger;
    SdLogger sdLogger;

    // To implementation:
    void initPeripherals();
    void measure();
    void rxNowHandler(const uint8_t *incomingData, int len);

    // Not to touch:
    void goToSleep();
    bool stateInWakenUp();
    void checkSleepThings();
};

extern Functions fcn;

#endif