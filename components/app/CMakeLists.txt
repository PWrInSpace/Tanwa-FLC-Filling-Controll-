idf_component_register(SRCS "app_init_task.c"  # List all source files here
                       INCLUDE_DIRS "."
                       REQUIRES device_config )      # Include the current directory for headers


# Apply compile options to the main component
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")
