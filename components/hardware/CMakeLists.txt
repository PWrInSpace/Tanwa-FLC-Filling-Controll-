idf_component_register(SRCS "pressure_driver.c" "ads1115.c" "max31856.c" "tmp1075.c"  # Your source files for device_config
                       INCLUDE_DIRS "."
                       REQUIRES driver)


# Apply compile options to the pinout component
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")
