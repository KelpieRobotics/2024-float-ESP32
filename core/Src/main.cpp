#include "main.h"

#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE //local log level
#include "esp_log.h"
#include "Hbridge.h"
#define LOG_TAG "MAIN" //for ESP logging inside main 

static Main my_main;

extern "C" void app_main(void) //linking because IDF expects this in C
{
    ESP_ERROR_CHECK(my_main.setup());

    while(true)
    {
        my_main.loop();
    }
}

esp_err_t Main::setup(void)
{
    esp_err_t status{ESP_OK};

    status |= h1.init();

    ESP_LOGI(LOG_TAG, "Setup status: %d", status);
    ESP_ERROR_CHECK(status);

    return status;
}

void Main::loop(void)
{
    h1.setForwards();
    vTaskDelay(pdSECOND);

    h1.setOff();
    vTaskDelay(pdSECOND);

    h1.setBackwards();
    vTaskDelay(pdSECOND);

    h1.setOff();
    vTaskDelay(pdSECOND);

}