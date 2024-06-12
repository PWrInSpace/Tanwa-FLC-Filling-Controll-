idf_component_register(SRCS "."
                       INCLUDE_DIRS ".")

# Apply compile options to the main component
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")
