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

static const char *tag = "LockStatus";

void sync_lock(){
   
    int wifi_res = wifi_connection();
    if (wifi_res == 0)
    {
        server_connection();
        send_to_server("test", "hello");
        vTaskDelay(pdMS_TO_TICKS(500));
      
    }
    server_disconnection();
    ESP_LOGI(tag, "Disconnected from the server");
     vTaskDelay(pdMS_TO_TICKS(500));
    wifi_disconnection();
    ESP_LOGI(tag, "Disconnected from the Wi-Fi");
     vTaskDelay(pdMS_TO_TICKS(500));
    

   esp_sleep_enable_timer_wakeup(1 * 1000000);
    esp_deep_sleep_start();
}

void app_main(void)
{
    uint8_t *data = 0;
    int8_t counter;
    time_t time;
    int wifi_res;
    int check_card_flag;

    init_spiffs();
   
    
    sync_lock();

}
