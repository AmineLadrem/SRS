#ifndef RFID_CONTROL_H
#define RFID_CONTROL_H

#include <stdio.h>
#include "driver/gpio.h"
#include "rom/gpio.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp32/rtc.h"
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "functions.h"

#define PIN_NUM 2
#define TX_PIN 5
#define RX_PIN 4
#define RTS (UART_PIN_NO_CHANGE)
#define CTS (UART_PIN_NO_CHANGE)
#define UART_PORT      1
#define BAUD_RATE     9600
#define STACK_SIZE    2048
#define BUF_SIZE (1024)

uart_config_t uart_config = {
    .baud_rate = BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

char* decode_data(uint8_t* data, int len) {
    char* hex_data = (char*)malloc((len * 2) + 1); 
    if (hex_data == NULL) {
        return NULL;
    }

    for (int i = 0; i < len; i++) {
        sprintf(hex_data + (i * 2), "%02x", data[i]);
    }
   
    hex_data[len * 2] = '\0';

    
    if (len >= 2 && (unsigned char)hex_data[0] == 'f' && isxdigit((unsigned char)hex_data[1])) {
    
    memmove(hex_data, hex_data + 2, len * 2 - 2);
    hex_data[len * 2 - 2] = '\0';
}


    return hex_data;
}


uint8_t *rfid_on(void) {
    int intr_alloc_flags = 0;
#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, TX_PIN, RX_PIN, RTS, CTS));

    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    if (data == NULL) {
        ESP_LOGE("RFID", "Failed to allocate memory for data buffer");
        uart_driver_delete(UART_PORT);
        return NULL; 
    }

    vTaskDelay(pdMS_TO_TICKS(1400)); // TO GIVE THE RFID READER ENOUGH TIME TO GET READY

    int total_wait_time_ms = 5000; // total wait time of 5 seconds
    int check_interval_ms = 100;   // check interval of 100 milliseconds
    int elapsed_time_ms = 0;
    bool card_detected=false;

    while (elapsed_time_ms < total_wait_time_ms) {
        int len = uart_read_bytes(UART_PORT, data, (BUF_SIZE - 1), check_interval_ms / portTICK_PERIOD_MS);  
        if (len > 4) {
            //card_passed_buzzer();
            data[len] = '\0';
            char* decoded_data = decode_data(data, len);
            ESP_LOGI("RFID", "Card No: %s\n", (char *)decoded_data); 
            uart_driver_delete(UART_PORT);
            card_detected=true;
            return (uint8_t *)decoded_data;
        }
        elapsed_time_ms += check_interval_ms;
    }

    uart_driver_delete(UART_PORT);
    ESP_LOGI("RFID", "No Card Passed");
    free(data);
    return NULL;
}


void rfid_off(){
     gpio_set_level(RFID_EN, 0);
     gpio_reset_pin(RFID_EN);
    gpio_reset_pin(UART);
}

#endif /* RFID_CONTROL_H */