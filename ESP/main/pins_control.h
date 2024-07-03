#ifndef PINS_CONTROL_H
#define PINS_CONTROL_H

#include <stdio.h>
#include "driver/gpio.h"
#include "rom/gpio.h"

//------------OUT PINS--------------
#define RFID_EN 2
#define UART 1
#define BUZZER 21
#define RED_LED 18
#define ADC_EN 6

#define LOCK_OUT1 19
#define LOCK_OUT2 20

//------------IN PINS--------------
#define LOCK_STATE 15
#define HAND_HOLD_STATE 14
#define CAPTEUR 7
#define Battery_LVL 3

int init_pins()
{


    gpio_pad_select_gpio(RFID_EN);
    gpio_set_direction(RFID_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(RFID_EN, 1);

    gpio_pad_select_gpio(UART);
    gpio_set_direction(UART, GPIO_MODE_OUTPUT);
    gpio_set_level(UART, 1);

    gpio_pad_select_gpio(BUZZER);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER, 0);

    gpio_pad_select_gpio(RED_LED);
    gpio_set_direction(RED_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(RED_LED, 1);

    gpio_pad_select_gpio(ADC_EN);
    gpio_set_direction(ADC_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(ADC_EN, 0);

    gpio_pad_select_gpio(LOCK_OUT1);
    gpio_set_direction(LOCK_OUT1, GPIO_MODE_OUTPUT);
    gpio_set_level(LOCK_OUT1, 0);

    gpio_pad_select_gpio(LOCK_OUT2);
    gpio_set_direction(LOCK_OUT2, GPIO_MODE_OUTPUT);
    gpio_set_level(LOCK_OUT2, 0);

    gpio_config_t io_conf;
    // Configure CAPTEUR pin as input with pull-up
    io_conf.intr_type = GPIO_INTR_POSEDGE; // Interrupt on positive edge
    io_conf.pin_bit_mask = (1ULL << CAPTEUR);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    gpio_pad_select_gpio(HAND_HOLD_STATE);
    gpio_set_direction(HAND_HOLD_STATE, GPIO_MODE_INPUT);

    gpio_pad_select_gpio(Battery_LVL);
    gpio_set_direction(Battery_LVL, GPIO_MODE_INPUT);

    return 0;
}

void deinit_pins()
{

    gpio_reset_pin(RFID_EN);
    gpio_reset_pin(UART);
    gpio_reset_pin(BUZZER);
    gpio_reset_pin(RED_LED);
    gpio_reset_pin(ADC_EN);
    gpio_reset_pin(LOCK_OUT1);
    gpio_reset_pin(LOCK_OUT2);

    gpio_reset_pin(LOCK_STATE);
    gpio_reset_pin(HAND_HOLD_STATE);
    gpio_reset_pin(Battery_LVL);
}

#endif /* PINS_CONTROL_H */