#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#define ANSWER_WAIT_MS 750
#define MIN_SLEEP_TIME_MS 2000
#define SLEEP_TIME_MS 30000

#include "pinout.h"
#include "stdint.h"
#include "Arduino.h"

// IMPORTANT! To set for each module and state:
enum Periods {

    INIT_PERIOD             = SLEEP_TIME_MS,
    IDLE_PERIOD             = SLEEP_TIME_MS,
    RECOVERY_ARM_PERIOD     = SLEEP_TIME_MS,
    FUELING_PERIOD          = 500,
    ARMED_TO_LAUNCH_PERIOD  = 1000,
    RDY_TO_LAUNCH_PERIOD    = 1000,
    COUNTDOWN_PERIOD        = 500,
    FLIGHT_PERIOD           = 10,
    FIRST_STAGE_REC_PERIOD  = 250,
    SECOND_STAGE_REC_PERIOD = 500,
    ON_GROUND_PERIOD        = SLEEP_TIME_MS,
    HOLD_PERIOD             = 1000,
    ABORT_PERIOD            = SLEEP_TIME_MS
};

// IMPORTANT END!

enum States {
  INIT = 0,
  IDLE,
  RECOVERY_ARM,
  FUELING,
  ARMED_TO_LAUNCH,
  RDY_TO_LAUNCH,
  COUNTDOWN,
  FLIGHT,
  FIRST_STAGE_RECOVERY,
  SECOND_STAGE_RECOVERY,
  ON_GROUND,
  HOLD,
  ABORT,
  NO_CHANGE = 0xff //DO NOT USE, ONLY FOR REQUEST PURPOSE
};

struct DataToSave {

    // IMPORTANT! To implementation of each module:



    String toString() {

        char outStr[50];
        sprintf(outStr, "test%d", 1);
        return String(outStr);
    }
    // IMPORTANT END!
};

struct DataToObc {

    bool wakenUp : 1;
    // IMPORTANT! To implementation of each module:

    // IMPORTANT END!
};

struct DataFromObc {

    uint32_t commandNum;
    int32_t commandArg;
};

struct ModuleData {

    DataToObc dataToObc;
    DataFromObc dataFromObc;
    DataToSave dataToSave;

    uint8_t obcState = 0;

    // IMPORTANT! Set the flash or sd logger:
    const bool flashSave = false;
    const bool sdSave = false;
    // IMPORTANT END!

    uint16_t stateTimes[13] = {
        INIT_PERIOD, IDLE_PERIOD, RECOVERY_ARM_PERIOD,
        FUELING_PERIOD, ARMED_TO_LAUNCH_PERIOD, RDY_TO_LAUNCH_PERIOD,
        COUNTDOWN_PERIOD, FLIGHT_PERIOD, FIRST_STAGE_REC_PERIOD,
        SECOND_STAGE_REC_PERIOD, ON_GROUND_PERIOD,
        HOLD_PERIOD, ABORT_PERIOD};

    // IMPORTANT! Add other global variables in this struct:

    // IMPORTANT END!
};

extern volatile ModuleData moduleData;

#endif