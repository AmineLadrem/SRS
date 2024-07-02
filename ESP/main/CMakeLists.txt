idf_component_register(SRCS "test.c" "main.c"
                    INCLUDE_DIRS ".")

spiffs_create_partition_image(lock_data /home/amineladrem/SRS/ESP/lock_data FLASH_IN_PROJECT)
