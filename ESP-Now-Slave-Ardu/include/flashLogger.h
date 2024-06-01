#ifndef FLASH_LOGGER_H
#define FLASH_LOGGER_H

#include "dataStructs.h"
#include "RTOS.h"
#include "LITTLEFS.h"

#define QUE_SIZE 30

class FlashLogger {

    static QueueHandle_t flashQueue;

public:

    void begin();
    void push();
    void read();

    static void logTask();
};

#endif