idf_component_register(SRCS "smartlight.c"
                    INCLUDE_DIRS ".")

# Add the 'index.html' file to the SPIFFS partition
spiffs_create_partition_image(storage ../spiffs_data FLASH_IN_PROJECT)