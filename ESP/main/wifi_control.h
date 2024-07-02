#ifndef WIFI_CONTROL_H
#define WIFI_CONTROL_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_spiffs.h"

char ssid[32];
char pass[64];
int retry_num = 0;
int wifi_connected = 0; 
esp_netif_t *my_sta = NULL;

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        printf("WIFI CONNECTING....\n");
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        printf("WiFi CONNECTED\n");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("WiFi lost connection\n");
        if (retry_num < 5) {
            esp_wifi_connect();
            retry_num++;
            printf("Retrying to Connect...\n");
        }
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        printf("Wifi got IP...\n\n");
        wifi_connected = 1; 

    }
}

int wifi_connection() {
    esp_netif_init();
    esp_event_loop_create_default();
    my_sta = esp_netif_create_default_wifi_sta();
    assert(my_sta);
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };

    char temp_ssid[32]; 
    char temp_pass[64]; 

    FILE *fp = fopen("/lock_data/lock_params.txt", "r");
    if (fp == NULL) {
        printf("Failed to open file for reading\n");
        return -1;
    }
    char line[100];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "ssid") != NULL) {
            sscanf(line, "ssid='%[^']'", temp_ssid);
        } else if (strstr(line, "pw") != NULL) {
            sscanf(line, "pw='%[^']'", temp_pass);
        }
    }
    fclose(fp);

    strcpy(ssid, temp_ssid);
    strcpy(pass, temp_pass);

    strcpy((char *)wifi_configuration.sta.ssid, ssid);
    strcpy((char *)wifi_configuration.sta.password, pass);
    printf("%s\n",(char *)wifi_configuration.sta.ssid);
    printf("%s\n",(char *)wifi_configuration.sta.password);

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);

    esp_wifi_connect();

    while (!wifi_connected) {
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }

    return 0; 
}



void wifi_disconnection() {
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
    if (my_sta != NULL) {
        esp_netif_destroy(my_sta);
        my_sta = NULL;
    }
    esp_event_loop_delete_default();
}

#endif /* WIFI_CONTROL_H */