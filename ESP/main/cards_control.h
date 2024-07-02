#ifndef CARDS_CONTROL_H
#define CARDS_CONTROL_H

#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include <string.h>


static const char *TAG = "lock";


void add_card(const char *card_number, uint64_t start_date, uint64_t end_date, unsigned int flag1, unsigned int flag2, unsigned int flag3) {
    // Check if card_number exceeds maximum length
    if (strlen(card_number) > 16) {
        ESP_LOGE(TAG, "Card number exceeds maximum length");
        return;
    }

    FILE *f = fopen("/lock_data/cards.txt", "a"); 
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open the file for appending");
        return;
    }

    fprintf(f, "%s,%llu,%llu,%u,%u,%u\n", card_number, start_date, end_date, flag1, flag2, flag3);
    
    fclose(f); 
}


int check_card(const char* data) {
    const char *file_path = "/lock_data/cards.txt";
    FILE *f = fopen(file_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open the file for reading");
        return 2; 
    }

    char line[128]; 
    while (fgets(line, sizeof(line), f) != NULL) {
        char card_number[32]; 
        uint64_t start_date, end_date; 
        unsigned int flag1, flag2, flag3; 
       
        sscanf(line, "%[^,],%llu,%llu,%u,%u,%u", card_number, &start_date, &end_date, &flag1, &flag2, &flag3);
    
        if (strcmp(card_number, data) == 0) {
            fclose(f); 
            if (flag1 == 0) {
                ESP_LOGI(TAG, "Card %s is valid", data);
                return 0; 
            } else if (flag1 == 1) {
                ESP_LOGI(TAG, "Card %s is blocked", data);
                return 1; 
            } else {
                ESP_LOGI(TAG, "Card %s is invalid", data);
                return 2;
            }
        }
    }

    fclose(f); 
    ESP_LOGI(TAG, "Card %s not found", data);
    return 2;
}




void read_cards_file() {
    const char *file_path = "/lock_data/cards.txt";
    FILE *f = fopen(file_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open the file for reading");
        return;
    }

    char line[128]; 
    while (fgets(line, sizeof(line), f) != NULL) {
        ESP_LOGI(TAG, "Line: %s", line);
    }

    fclose(f); 
}

void add_log(const char *card_number, time_t time) {
    const char *file_path = "/lock_data/log.txt";
    FILE *f = fopen(file_path, "a+"); 
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open the file for appending");
        return;
    }

    
    int result = fprintf(f, "%s,%ld\n", card_number, (long)time);
    if (result < 0) {
        ESP_LOGE(TAG, "Failed to write to log file");
    }
    
    fclose(f); 
}



void read_log() {
    const char *file_path = "/lock_data/log.txt";
    FILE *f = fopen(file_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open the file for reading");
        return;
    }

    char line[128]; 
    while (fgets(line, sizeof(line), f) != NULL) {
        ESP_LOGI(TAG, "Line: %s", line);
    }

    fclose(f); 
}


void read_cards() {
    const char *file_path = "/lock_data/cards.txt";
    FILE *f = fopen(file_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open the file for reading");
        return;
    }

    char line[128]; 
    while (fgets(line, sizeof(line), f) != NULL) {
        ESP_LOGI("Card Line", "Line: %s", line);
    }

    fclose(f); 
}



int check_rfid_data(const char* data){

    return strlen(data) ;
}

#endif /* CARDS_CONTROL_H */