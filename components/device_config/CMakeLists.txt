idf_component_register(SRCS "TANWA_FLC_config.c" "mcu_twai_config.c"  # Your source files for device_config
                       INCLUDE_DIRS "."
                       REQUIRES hardware mcu_config)

