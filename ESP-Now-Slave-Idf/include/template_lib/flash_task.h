#ifndef FLASH_TASK_H
#define FLASH_TASK_H

#include "dataStructs.h"

bool initFlashTask(void);
bool sendDataToFlashTask(DataToSave *data);
void run_memory_tasks(void);
#endif