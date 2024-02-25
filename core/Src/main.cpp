#include "main.h"

#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE //local log level
#include "esp_log.h"
#define LOG_TAG "MAIN" //for ESP logging inside main 

extern "C" void app_main(void) //linking because IDF expects this in C
{
    while(true){
        ESP_LOGI(LOG_TAG, "Hello World!");
        vTaskDelay(1000);
    }
}