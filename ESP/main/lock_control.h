#ifndef LOCK_CONTROL_H
#define LOCK_CONTROL_H

#include <stdio.h>
#include "driver/gpio.h"
#include "rom/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <inttypes.h>
#include "esp_sleep.h"

#define LOCK_OUT1 19
#define LOCK_OUT2 20

#define LOCK_STATE 15
#define CAPTEUR 7
#define HAND_HOLD_STATE 14

void lock_deep_sleep_2();

void open_lock()
{
    gpio_set_level(LOCK_OUT1, 0);
    gpio_set_level(LOCK_OUT2, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(LOCK_OUT1, 0);
    gpio_set_level(LOCK_OUT2, 0);
}

void close_lock()
{
    gpio_set_level(LOCK_OUT1, 1);
    gpio_set_level(LOCK_OUT2, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(LOCK_OUT1, 0);
    gpio_set_level(LOCK_OUT2, 0);
}

int check_lock_state() {
    return (gpio_get_level(LOCK_STATE));
}

int check_lock_state2() {
    return (gpio_get_level(HAND_HOLD_STATE));
}

int check_capteur_state() {
    return (gpio_get_level(CAPTEUR));
}

void lock_deep_sleep(){
 esp_sleep_enable_timer_wakeup(2 * 1000000);
    esp_deep_sleep_start();
}

void lock_deep_sleep_2()
{
    printf("Lock is closed. Going to deep sleep.\n");
            esp_sleep_enable_ext1_wakeup((1ULL << CAPTEUR), 1);
            esp_deep_sleep_start();
}

#endif /* LOCK_CONTROL_H */