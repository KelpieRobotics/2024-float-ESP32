/*
    Ethan Bowering @ Kelpie Robotics
    May 2024
*/

#include "Selkie.h"

//#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE //local log level
#include "esp_log.h"
#include "Hbridge.h"
#include "packet.h"
#include <ctime>
#define LOG_TAG "MAIN" //for ESP logging inside main 

extern "C" void app_main(void) //linking because IDF expects this in C
{

    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initialising NVS");
    ESP_ERROR_CHECK(nvs_flash_init());
    
    ESP_ERROR_CHECK(setup());

    wifi_connect();

   

    //TaskHandle_t xHandle = NULL;

    //xTaskCreate(record_data_task, "test", 4096, NULL, 5, &xHandle);

    //vTaskDelay(10*pdSECOND);

    //vTaskDelete(xHandle);
    
}

esp_err_t setup(void)
{
    esp_err_t status{ESP_OK};

    //status |= h1.init();

    //status |= adc_unit.init();

    //status |= pressure_sens.init();

    //status |= i2c_ctrl.init();

    //status |= psi_snsr.init();

    status |= wifi.init();

    status = esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &ip_event_handler,
                                                            nullptr,
                                                            nullptr);

    ESP_LOGI(LOG_TAG, "Setup status: %d\n", status);
    ESP_ERROR_CHECK(status);

    return status;
}

void loop(void)
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

    ESP_LOGI(LOG_TAG, "Wifi begin");
    wifi.begin();
    ESP_LOGI(LOG_TAG, "Blocking???");
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

esp_err_t wifi_connect()
{
    return(wifi.begin());
}

void record_data_task(void* pvParameters)
{
    while(true)
   { 
        psi_snsr.read();
        packet_t packet{time(NULL), psi_snsr.pressure(), psi_snsr.depth()}; //company number, time, pressure, depth
        //packet_t packet{COMPANY_NUMBER, time(NULL), 100, 100}; //company number, time, pressure, depth
        ESP_LOGD(LOG_TAG, "PACKET: NUMBER: %i, TIME: %i, PRESSURE: %f, DEPTH: %f", packet.company_number, (int) packet.time, packet.pressure, packet.depth); 
        data.push_back(packet);
        vTaskDelay(5*pdSECOND); //5s delay per manual
    }
}

void dive_task(void* pvParameters)
{
    h1.setForwards();
    ESP_LOGD(LOG_TAG, "Diving...");
    vTaskDelay(10*pdSECOND);
    h1.setOff();
    ESP_LOGD(LOG_TAG, "Emptied tank");
    vTaskDelete(NULL);
    
}
//should probably make these two one task with a long delay in between them
void surface_task(void* pvParameters)
{
    h1.setBackwards();
    ESP_LOGD(LOG_TAG, "Surfacing...");
    vTaskDelay(10*pdSECOND);
    h1.setOff();
    ESP_LOGD(LOG_TAG, "Filled tank");
    vTaskDelete(NULL);
}

void ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    ESP_ERROR_CHECK(tcp_client.socket_connect());
    if (!first_packet)
    {
        //create 1st packet if this is first call
        data.push_back(packet_t{time(NULL), 1234, 1234});
        first_packet = true;  
    }

    vTaskDelay(5*pdSECOND);                                             //garbage test data
    data.push_back(packet_t{time(NULL), 1234, 1234});
    vTaskDelay(5*pdSECOND);
    data.push_back(packet_t{time(NULL), 1234, 1234});
    vTaskDelay(5*pdSECOND);
    data.push_back(packet_t{time(NULL), 1234, 1234});

    std::list<packet_t>::iterator it; //iterate through and send all packets
    for (it = data.begin(); it != data.end(); it++)
    {
        ESP_LOGD(LOG_TAG, "%s", it->toString().c_str());
        tcp_client.socket_send(it->toString());
    }
    data.clear(); //clear for dive

    std::string msg{};
    tcp_client.socket_receive(msg);
    ESP_LOGD(LOG_TAG, "%s", msg.c_str());

    tcp_client.socket_disconnect();

    wifi.end();
    vTaskDelay(5*pdSECOND);
    wifi.begin();

    return;
}