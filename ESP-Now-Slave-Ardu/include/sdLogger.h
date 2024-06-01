#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include "dataStructs.h"
#include "RTOS.h"
#include "SPI.h"
#include "sd_diskio.h"
#include <FS.h>
#include "SD.h"

#define QUE_SIZE 30

class SdLogger {

    static QueueHandle_t sdQueue;

    static bool sdWrite(const String &path, const String &dataFrame);

public:

    void begin();
    void push();
    static void logTask();
};

#endif