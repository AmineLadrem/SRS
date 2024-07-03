#ifndef BATTERY_H
#define BATTERY_H

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_cali.h"

#define ADC_EN 6

static adc_oneshot_unit_handle_t handle = NULL;
static adc_cali_handle_t cali_handle = NULL;

void adc_init(void)
{

    // Initialize the ADC unit
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &handle));

    // Configure the ADC channel
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_CHANNEL_3, &config));

    // Set up ADC calibration
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle));
}

int get_adc_voltage(void)
{
    int adc_reading, mv_output;
    gpio_set_level(ADC_EN, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(adc_oneshot_read(handle, ADC_CHANNEL_3, &adc_reading));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_reading, &mv_output));
    gpio_set_level(ADC_EN, 0);
    return mv_output * 3;
}

#endif // BATTERY_H
