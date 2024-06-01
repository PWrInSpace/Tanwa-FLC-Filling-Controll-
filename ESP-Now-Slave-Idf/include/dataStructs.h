#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <stdbool.h>
#include <stdint.h>
#include "pinout.h"

#define ANSWER_WAIT_MS 750
#define MIN_SLEEP_TIME_MS 2000
#define SLEEP_TIME_MS 30000

// CONFIGURE MEMORY LOGGING
#define DATA_LOG_PERIOD 100  // ms // IMPORTANT!
#define DATA_QUEUE_SIZE 40
#define DATA_ONE_SHOT_SAVE_NB 20
#if DATA_ONE_SHOT_SAVE_NB >= DATA_QUEUE_SIZE
    #error Number of one shot save data is greater than queue size
#endif

#define SD_SPI_HOST VSPI_HOST

#define SD_FILE_NAME "sd_data"
#define SD_HEADER "TEST1234\n"
#define SD_BUFFER_SIZE 100

#define FLASH_FILE_NAME "flash"
#define STATE_NUM 13

// IMPORTANT! To set for each module and state:
typedef enum {

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
} Periods;

// IMPORTANT END!

typedef enum {
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
} States;


typedef struct DataToObc {

    bool wakenUp : 1;
    // IMPORTANT! To implementation of each module:

    // IMPORTANT END!
} DataToObc;

typedef struct {

    uint32_t commandNum;
    int32_t commandArg;

} DataFromObc;

typedef struct DataToSave {

    uint32_t uptime;
    // IMPORTANT! To implementation of each module:

    // IMPORTANT END!
} DataToSave;

typedef struct {

    DataToObc dataToObc;
    DataFromObc dataFromObc;
    DataToSave dataToSave;
    uint8_t obcState;
    uint16_t stateTimes[13];
    bool inServiceMode;

    // IMPORTANT! Add other global variables in this struct:

    // IMPORTANT END!
} ModuleData;

extern volatile ModuleData moduleData;

#endif