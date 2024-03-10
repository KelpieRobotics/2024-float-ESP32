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

    status |= adc_unit.init();

    status |= pressure_sens.init();

    ESP_LOGI(LOG_TAG, "Setup status: %d", status);
    ESP_ERROR_CHECK(status);

    pressure = 0;

    return status;
}

void Main::loop(void)
{
    //pressure_sens.read(&pressure);

    ESP_LOGD(LOG_TAG, "size of uint8_t: %d", sizeof(uint8_t));
    ESP_LOGD(LOG_TAG, "size of const uint8_t: %d", sizeof(const uint8_t));

    ESP_LOGD(LOG_TAG, "size of uint16_t: %d", sizeof(uint16_t));
    ESP_LOGD(LOG_TAG, "size of const uint16_t: %d", sizeof(const uint16_t));

    uint8_t r_buf[2] = {0};
    ESP_LOGD(LOG_TAG, "size of buf: %d", sizeof(r_buf));

    vTaskDelay(pdSECOND);
    /*
    h1.setForwards();
    vTaskDelay(pdSECOND);

    h1.setOff();
    vTaskDelay(pdSECOND);

    h1.setBackwards();
    vTaskDelay(pdSECOND);

    h1.setOff();
    vTaskDelay(pdSECOND);
    */
}