#ifndef SPIFFS_CONTROL_H
#define SPIFFS_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include "esp_spiffs.h"

esp_vfs_spiffs_conf_t conf = {
    .base_path = "/lock_data",
    .partition_label = NULL,
    .max_files = 10,
    .format_if_mount_failed = true,
};

void init_spiffs() {
    nvs_flash_init();
    esp_vfs_spiffs_register(&conf);
}

#endif /* SPIFFS_CONTROL_H */
