#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdio.h>
#include "driver/gpio.h"
#include "rom/gpio.h"


#define RED_LED 6
#define GREEN_LED 18

void TURN_RED_LED(){
    gpio_set_level(RED_LED, 0);
    gpio_set_level(GREEN_LED, 1);
 
}

void TURN_GREEN_LED(){
    gpio_set_level(RED_LED, 1);
    gpio_set_level(GREEN_LED, 0);
   
}


void TURN_OFF_LED(){
    gpio_set_level(RED_LED, 1);
    gpio_set_level(GREEN_LED, 1);
   
}

#endif /* LED_CONTROL_H */