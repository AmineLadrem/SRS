#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "rom/gpio.h"
#include "esp_sleep.h"
#include "esp32/rtc.h"
#include <time.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "driver/adc.h"
#include "wifi_control.h"
#include "server_control.h"

#define BUZZER 21

typedef struct
{
    char *key;
    int8_t data;
    size_t size;
} data_item_t;

gpio_num_t adc_gpio_num;

esp_err_t write_data(nvs_handle_t handle, const char *key, int8_t value);

int8_t read_data(nvs_handle_t handle, data_item_t *item)
{
    int8_t ds_counter, counter = 0;
    esp_err_t err = nvs_get_i8(handle, item->key, &ds_counter);
    switch (err)
    {
    case ESP_OK:
        counter = ds_counter;
        if (ds_counter == 0)
        {
            ds_counter = 2;
        }
        else
        {
            ds_counter--;
        }
        write_data(handle, item->key, ds_counter);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        break;
    default:
        break;
    }
    return counter;
}

esp_err_t write_data(nvs_handle_t handle, const char *key, int8_t value)
{
    esp_err_t err = nvs_set_i8(handle, key, value);
    return err;
}

esp_err_t open_nvs(nvs_handle_t *handle)
{
    esp_err_t err = nvs_open("storage", NVS_READWRITE, handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        // printf("Done\n");
    }
    return err;
}

esp_err_t close_nvs(nvs_handle_t handle)
{
    nvs_close(handle);
    return ESP_OK;
}

esp_err_t init_nvs()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    return err;
}

int8_t decrement_counter()
{

    int8_t counter = 0;
    init_nvs();

    nvs_handle_t nvs_handle;
    open_nvs(&nvs_handle);

    data_item_t item = {
        .key = "ds_counter",
        .data = 0,
        .size = sizeof(item.data)};

    esp_err_t err = nvs_get_i8(nvs_handle, item.key, &item.data);
    if (err != ESP_OK)
    {
        err = nvs_set_i8(nvs_handle, item.key, 4);
        if (err == ESP_OK)
        {
            // printf("Done\n");
        }
        else
        {
            printf("Error (%s) creating key!\n", esp_err_to_name(err));
        }
    }
    else
    {
        counter = read_data(nvs_handle, &item);
    }

    close_nvs(nvs_handle);
    return counter;
}

void card_passed_buzzer()
{
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER, 1);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    gpio_set_level(BUZZER, 0);
}

void deep_sleep_buzzer()
{
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER, 1);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gpio_set_level(BUZZER, 0);
      vTaskDelay(500 / portTICK_PERIOD_MS);
    gpio_set_level(BUZZER, 1);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gpio_set_level(BUZZER, 0);
}

int get_battery_level()
{
    adc1_pad_get_io_num(ADC1_CHANNEL_3, &adc_gpio_num);
    printf("ADC GPIO number: %d\n", adc_gpio_num);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);

    int raw_value = adc1_get_raw(ADC1_CHANNEL_3);
    printf("Raw ADC reading: %d\n", raw_value);

    return raw_value;
}

void rtc_sync()
{

    int wifi_res = wifi_connection();

    if (wifi_res == 0)
    {
        server_connection();
        send_to_server("get_cards", "1");
        vTaskDelay(pdMS_TO_TICKS(300));
        server_disconnection();
        server_connection();
        send_to_server("get_time", "1");
        vTaskDelay(pdMS_TO_TICKS(300));
        server_disconnection();
        wifi_disconnection();
    }
}

#endif /* FUNCTIONS_H */