#ifndef RTC_CONTROL_H
#define RTC_CONTROL_H

#include <time.h>
#include <sys/time.h>
#include "esp32/rtc.h"
#include "functions.h"

void rtc_sync();


int set_time(const char *epoch_time_str) {
    time_t epoch_time = atoi(epoch_time_str);
    
    struct timeval tv;

    tv.tv_sec = epoch_time; 
    tv.tv_usec = 0;

    if (settimeofday(&tv, NULL) == 0) {
        printf("System time updated successfully.\n");
        return 0;
    } else {
        printf("Failed to update system time.\n");
        return -1;
    }
}


time_t get_time(){
    struct tm timeinfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);
    time_t epoch_time = mktime(&timeinfo);
    return epoch_time;
}


int check_rtc(){
    time_t now = get_time();
     
    printf("Current time: %lu\n", (unsigned long)now);

    if(now<10000){
        return 0;
    }
    else {
        return 1;
    }

}

#endif /* RTC_CONTROL_H */