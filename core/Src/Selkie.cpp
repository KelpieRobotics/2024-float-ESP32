#include "Selkie.h"

#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE //local log level
#include "esp_log.h"
#include "Hbridge.h"
#include "packet.h"
#define LOG_TAG "MAIN" //for ESP logging inside main 

static Selkie scout;

Selkie::state_e Selkie::_state{state_e::NOT_INITIALISED};
std::list<packet_t> Selkie::data{};

extern "C" void app_main(void) //linking because IDF expects this in C
{

    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initialising NVS");
    ESP_ERROR_CHECK(nvs_flash_init());
    
    ESP_ERROR_CHECK(scout.setup());

    while(true)
    {
        scout.loop();
    }
}

esp_err_t Selkie::setup(void)
{
    esp_err_t status{ESP_OK};

    //status |= h1.init();

    //status |= adc_unit.init();

    //status |= pressure_sens.init();

    //status |= i2c_ctrl.init();

    //status |= psi_snsr.init();

    status |= wifi.init();

    ESP_LOGI(LOG_TAG, "Setup status: %d\n", status);
    ESP_ERROR_CHECK(status);

    return status;
}

void Selkie::loop(void)
{

/*
    ESP_ERROR_CHECK(psi_snsr.read());

    pressure = psi_snsr.pressure();

    temp = psi_snsr.temperature();

    depth = psi_snsr.depth();

    altitude = psi_snsr.altitude();

    ESP_LOGD(LOG_TAG, "Pressure: %f", pressure);

    ESP_LOGD(LOG_TAG, "Temperature: %f", temp);

    ESP_LOGD(LOG_TAG, "Depth: %f", depth);

    ESP_LOGD(LOG_TAG, "Altitude: %f\n", altitude);
*/

    wifi.begin();
    ESP_LOGI(LOG_TAG, "----------------------------------------------");
    vTaskDelay(10*pdSECOND);
    wifi.end();
    ESP_LOGI(LOG_TAG, "----------------------------------------------");
    vTaskDelay(10*pdSECOND);

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

esp_err_t Selkie::wifi_connect()
{
    return(wifi.begin());
}

void Selkie::record_data()
{
    psi_snsr.read();
    packet_t packet{NULL, NULL, psi_snsr.pressure(), psi_snsr.depth()}; //company number, time, pressure, depth
    data.push_back(packet);

}
