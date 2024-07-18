idf_component_register(SRCS "measure_task.c" "rtos_utilities.c" "app_init_task.c" "can_task.c" # List all source files here
                       INCLUDE_DIRS "."
                       REQUIRES device_config hardware mcu_config)      # Include the current directory for headers


# Apply compile options to the main component
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")
