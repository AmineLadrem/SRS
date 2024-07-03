#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "functions.h"
#include "led_control.h"
#include "pins_control.h"
#include "rfid_conf.h"
#include "cards_control.h"
#include "rtc_control.h"
#include "wifi_control.h"
#include "server_control.h"
#include "lock_control.h"
#include "spiffs_control.h"
#include "battery.h"

#define LOG_BUFFER_SIZE 2048

static const char *tag = "LockStatus";
char log_buffer[LOG_BUFFER_SIZE];
char battery_log[100];
int battery_level;

void add_to_log(const char *message);

void sync_lock()
{
    int wifi_res = wifi_connection();
    if (wifi_res == 0)
    {
        add_to_log("Connected to the Wi-Fi");
        server_connection();
        add_to_log("Connected to the Server");
        send_to_server("get_cards", "300");
        vTaskDelay(pdMS_TO_TICKS(100));
        send_to_server("battery", battery_log);
        send_log_to_server();
        vTaskDelay(pdMS_TO_TICKS(100));
        add_to_log("disconnect from the server and Wi-Fi");
        add_to_log("Deepsleep");
        send_to_server("log_esp", log_buffer);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    server_disconnection();
    ESP_LOGI(tag, "Disconnected from the server");
    wifi_disconnection();
    ESP_LOGI(tag, "Disconnected from the Wi-Fi");
}

void rtc_sync()
{

    int wifi_res = wifi_connection();

    if (wifi_res == 0)
    {
        server_connection();
        send_to_server("get_cards", "1");
        vTaskDelay(pdMS_TO_TICKS(100));
        send_to_server("get_time", "1");
        vTaskDelay(pdMS_TO_TICKS(100));
        add_to_log("Turn on for the first time");
        add_to_log("RTC synchronized");
        add_to_log("disconnect from the server and Wi-Fi");
        send_to_server("log_esp", log_buffer);
        vTaskDelay(pdMS_TO_TICKS(100));
        server_disconnection();
        wifi_disconnection();

        lock_deep_sleep(0.5);
    }
}

void add_to_log(const char *message)
{
    struct timeval now;
    struct tm timeinfo;
    char time_str[64];

    // Get the current time
    gettimeofday(&now, NULL);
    localtime_r(&now.tv_sec, &timeinfo);

    // Format the time as a string with milliseconds
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);
    char full_time_str[80];
    snprintf(full_time_str, sizeof(full_time_str), "%s.%03ld", time_str, now.tv_usec / 1000);

    // Create the log message with timestamp
    char log_entry[256]; // Adjust size as needed
    snprintf(log_entry, sizeof(log_entry), "[%s] %s", full_time_str, message);

    // Append the log entry to the log buffer
    strncat(log_buffer, log_entry, LOG_BUFFER_SIZE - strlen(log_buffer) - 1);
    strncat(log_buffer, "\n", LOG_BUFFER_SIZE - strlen(log_buffer) - 1);
}

void app_main(void)
{
    uint8_t *data = 0;
    int8_t counter;
    time_t time;
    int wifi_res, check_card_flag;

    init_spiffs();
    adc_init();
    init_pins();
    if (check_rtc() == 0)
    {
        rtc_sync();
    }
    add_to_log("ESP initialized");
    battery_level=get_adc_voltage();
    snprintf(battery_log, sizeof(battery_log), "Battery Level: %d", battery_level);
    add_to_log(battery_log);

    TickType_t start_time = xTaskGetTickCount();

    while (check_capteur_state())
    {

        ESP_LOGI(tag, "Going to read the cards");
        data = rfid_on();

        if (data != NULL && check_rfid_data((const char *)data))
        {
            add_to_log("A card was detected");
            time = get_time();
            check_card_flag = check_card((const char *)data);
            read_cards();

            if (check_card_flag == 0)
            { // Valid Card
                add_to_log("Valid card");
                TURN_GREEN_LED();
                open_lock();
                add_to_log("Open the lock");
                card_passed_buzzer();
                vTaskDelay(pdMS_TO_TICKS(5700));
                close_lock();
                add_to_log("Close the lock");
                TURN_OFF_LED();
            }
            else if (check_card_flag == 1 || check_card_flag == 2)
            { // Blocked Card
                add_to_log("Blocked / Invalid card");
                TURN_RED_LED();
                card_passed_buzzer();
                vTaskDelay(pdMS_TO_TICKS(700));
                TURN_OFF_LED();
            }
            add_log((const char *)data, (long)time);
            read_log();

            counter = decrement_counter();
            printf("Counter = %" PRIu8 "\n", counter);
        }
        else
        {
            ESP_LOGI(tag, "No RFID data found or invalid");
        }

        // Reset data to NULL to ensure the next loop iteration calls rfid_on() again.
        data = NULL;
    }
    sync_lock();

    TickType_t end_time = xTaskGetTickCount();
    TickType_t elapsed_time = end_time - start_time;
    float time_milliseconds = ((float)elapsed_time * 1000) / configTICK_RATE_HZ;
    ESP_LOGI(TAG, "Time taken: %.2f milliseconds", time_milliseconds);

    rfid_off();
    deinit_pins();
    lock_deep_sleep_2();
}
