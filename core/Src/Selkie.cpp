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

TaskHandle_t xHandle = NULL;

extern "C" void app_main(void) //linking because IDF expects this in C
{

    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initialising NVS");
    ESP_ERROR_CHECK(nvs_flash_init());
    
    ESP_ERROR_CHECK(setup());

    wifi.begin();
    
}

esp_err_t setup(void)
{
    esp_err_t status{ESP_OK};

    status |= h1.init();

    status |= i2c_ctrl.init();

    status |= psi_snsr.init();

    status |= wifi.init();

    status = esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &ip_event_handler,
                                                            nullptr,
                                                            nullptr);

    data.push_back(packet_t{time(NULL), 1234, 1234});

    ESP_LOGI(LOG_TAG, "Setup status: %d\n", status);
    ESP_ERROR_CHECK(status);

    return status;
}


esp_err_t wifi_connect()
{
    return(wifi.begin());
}

void record_data_task(void* pvParameters)
{
    int ctr = 0;
    float current_velocity = 0;
    
    psi_snsr.read();
    float pressure = psi_snsr.pressure();
    float depth = psi_snsr.depth();
    
    time_t current_time = time(NULL);
    depth_history.push_back({current_time, depth, current_velocity});

    while(true)
   { 
        vTaskDelay(pdMS_TO_TICKS(500)); //two times per second, change this to xTaskDelayUntil
        ctr += 500;

        psi_snsr.read();
        pressure = psi_snsr.pressure();
        depth = psi_snsr.depth();

        current_velocity = velocity(std::get<1>(depth_history.back()), depth);
        current_time = time(NULL);
        depth_history.push_back({current_time, depth, current_velocity});

        if (ctr >= 5000) //every 5 seconds 
        {
            packet_t packet{current_time, pressure, depth}; //company number, time, pressure, depth
            ESP_LOGI(LOG_TAG, "%s", packet.to_string().c_str());
            data.push_back(packet);
            ctr = 0;
        }
    }
}

float velocity(float v1, float v2) //metres per second
{
    return (v2-v1)/0.5;
}

void dive_task(void* pvParameters)
{
    h1.setForwards();
    ESP_LOGI(LOG_TAG, "Diving...");
    vTaskDelay(10*pdSECOND);
    h1.setOff();
    ESP_LOGI(LOG_TAG, "Emptied tank");
    vTaskDelete(NULL);   
}

void test_dive_task(void* pvParameters)
{
    h1.setForwards();
    ESP_LOGI(LOG_TAG, "Diving...");
    vTaskDelay(120*pdSECOND);
    h1.setOff();
    ESP_LOGI(LOG_TAG, "Emptied tank");
    vTaskDelay(5*pdSECOND);

    h1.setBackwards();
    ESP_LOGI(LOG_TAG, "Surfacing...");
    vTaskDelay(120*pdSECOND);
    h1.setOff();
    ESP_LOGI(LOG_TAG, "Filled tank");
    vTaskDelay(5*pdSECOND);
    vTaskDelete(xHandle);

    wifi.begin();
    vTaskDelete(NULL);}

//should probably make these two one task with a long delay in between them
void surface_task(void* pvParameters)
{
    h1.setBackwards();
    ESP_LOGI(LOG_TAG, "Surfacing...");
    vTaskDelay(10*pdSECOND);
    h1.setOff();
    ESP_LOGI(LOG_TAG, "Filled tank");
    vTaskDelete(NULL);
}

void ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    ESP_ERROR_CHECK(tcp_client.socket_connect());

    std::list<packet_t>::iterator it; //iterate through and send all packets
    for (it = data.begin(); it != data.end(); it++)
    {
        ESP_LOGI(LOG_TAG, "%s", it->to_string().c_str());
        tcp_client.socket_send(it->to_string());
    }
    data.clear(); //clear for dive

    ESP_LOGI(LOG_TAG, "Waiting for command...");
    std::string msg{};
    tcp_client.socket_receive(msg);
    ESP_LOGI(LOG_TAG, "%s", msg.c_str());

    tcp_client.socket_disconnect();
    wifi.end();
   
    xTaskCreate(record_data_task, "Data recording task", 4096, NULL, 5, &xHandle); //change priority and stack
    xTaskCreate(test_dive_task, "Dive task", 4096, NULL, 5, NULL);

    return;
}

//bottom event handler

//surface event handler