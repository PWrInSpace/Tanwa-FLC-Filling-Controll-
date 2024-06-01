#ifndef CMD_H
#define CMD_H

#include "esp_console.h"
#include "functions.h"


// For some serial monitors this lib is crashing
// Works with cutecom ...

/**
 * @brief All in one initialization
 *
 * @return esp_err_t status
 */
esp_err_t CMD_initialize(void);

/**
 * @brief All in one initialization
 *
 * @return esp_err_t status
 */
esp_err_t CMD_deinitialize(void);


#endif