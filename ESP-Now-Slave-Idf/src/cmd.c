#include "cmd.h"
#include "esp_system.h"

//#ifdef TEMPLATE_FLASH
    #include "template_lib/flash.h"
//#endif

#include "dataStructs.h"
#include "esp_log.h"

#ifdef TEMPLATE_CMD
/****** COMMANDS ******/

/**
 * @brief Reset device command
 *
 * @param argc number of arguments
 * @param argv array of arguments
 * @return int status code 0 - :), other - :C
 */
static int CMD_reset_dev(int argc, char **argv) {
    esp_restart();
    return 0;
}

static int CMD_change_state(int argc, char **argv) {
    uint8_t state = moduleData.obcState;
    if (argc == 2) {
        state = atoi(argv[1]);
        moduleData.obcState = state;
    } else {
        moduleData.obcState = (int)state + 1;
    }

    return 0;
}

static int CMD_turn_on_service_mode(int argc, char **argv) {
    moduleData.inServiceMode = true;

    return 0;
}


static int CMD_turn_on_normal_mode(int argc, char **argv) {
    moduleData.inServiceMode = false;

    return 0;
}

#ifdef TEMPLATE_FLASH

static int CMD_flash_erase(int argc, char **argv) {
    if (FLASH_format() != FLASH_OK) {
        return -1;
    }

    return 0;
}

#pragma message "Remember to update write format"
static int CMD_flash_read(int argc, char **argv) {
    esp_log_level_set("*", ESP_LOG_NONE);
    FILE *file = NULL;
    file = fopen(FLASH_CREATE_PATH(FLASH_FILE_NAME), "r");
    if (file == NULL) {
        return -1;
    }

    DataToSave data;
    while(fread(&data, sizeof(data), 1, file)) {
        char data_buffer[100];
        create_sd_string(&data, data_buffer, sizeof(data_buffer));
    }
    fclose(file);

    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(TAG, "Read end");
    return 0;
}

#endif

// Array of commands [command name, help text, hint text, function, argtable]
static esp_console_cmd_t commands[] = {
    {"reset-dev", "restart device", NULL, CMD_reset_dev, NULL},
    {"state", "change state", NULL, CMD_change_state, NULL},
    {"service-on", "turn off sleep mode", NULL, CMD_turn_on_service_mode, NULL},
    {"service-off", "turn on sleep mode", NULL, CMD_turn_on_normal_mode, NULL},
#ifdef TEMPLATE_FLASH
    {"flash-erase", "erase flash memory", NULL, CMD_flash_erase ,NULL},
    {"flash-read", "read flash memory", NULL, CMD_flash_read, NULL},
#endif
};


/****** PRIVATE ******/

static esp_err_t console_init(void) {
    esp_err_t ret;

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = ">";
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ret = esp_console_new_repl_uart(&hw_config, &repl_config, &repl);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_console_register_help_command();
    if (ret != ESP_OK) {
        return ret;
    }

    esp_console_start_repl(repl);
    return ret;
}

static esp_err_t console_register_commands(esp_console_cmd_t *commands, size_t number_of_cmd) {
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < number_of_cmd; ++i) {
        ret = esp_console_cmd_register(&commands[i]);
        if (ret != ESP_OK) {
            return ret;
        }
    }

    return ret;
}

esp_err_t CMD_initialize(void) {
    esp_err_t res;
    res = console_init();
    if (res != ESP_OK) {
        return res;
    }

    res = console_register_commands(commands, sizeof(commands)/sizeof(commands[0]));

    return res;
}

esp_err_t CMD_deinitialize(void) {
    return esp_console_deinit();
}

#endif