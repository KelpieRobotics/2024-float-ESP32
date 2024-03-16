#include "main.h"

#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE //local log level
#include "esp_log.h"
#include "Hbridge.h"
#define LOG_TAG "MAIN" //for ESP logging inside main 

static Main my_main;

extern "C" void app_main(void) //linking because IDF expects this in C
{

    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initialising NVS");
    ESP_ERROR_CHECK(nvs_flash_init());
    
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

    //status |= adc_unit.init();

    //status |= pressure_sens.init();

    status |= i2c_ctrl.init();

    status |= psi_snsr.init();

    ESP_LOGI(LOG_TAG, "Setup status: %d\n", status);
    ESP_ERROR_CHECK(status);

    return status;
}

void Main::loop(void)
{
    //pressure_sens.read(&pressure);

    ESP_ERROR_CHECK(psi_snsr.read());

    pressure = psi_snsr.pressure();

    temp = psi_snsr.temperature();

    depth = psi_snsr.depth();

    altitude = psi_snsr.altitude();

    ESP_LOGD(LOG_TAG, "Pressure: %f", pressure);

    ESP_LOGD(LOG_TAG, "Temperature: %f", temp);

    ESP_LOGD(LOG_TAG, "Depth: %f", depth);

    ESP_LOGD(LOG_TAG, "Altitude: %f\n", altitude);

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