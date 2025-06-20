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

TaskHandle_t recordTaskHandle = NULL;
TaskHandle_t motorTaskHandle = NULL;

extern "C" void app_main(void) //linking because IDF expects this in C
{

    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initialising NVS");
    ESP_ERROR_CHECK(nvs_flash_init());
    
    ESP_ERROR_CHECK(setup());

    xTaskCreate(record_data_task, "Data recording task", 4096, NULL, 5, &recordTaskHandle); //change priority and stack
    xTaskCreate(test_dive_task, "Dive task", 4096, NULL, 5, &motorTaskHandle);

    wifi.begin();


    
}

esp_err_t setup(void)
{
    esp_err_t status{ESP_OK};

    status |= h1.init();

    status |= i2c_ctrl.init();

    status |= psi_snsr.init();

    status |= wifi.init();

    status |= esp_event_handler_instance_register(IP_EVENT,
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
    while (true)
    {
        int state = 0;
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //wait for notification to dive

        int ctr = 0;
        float current_velocity = 0;
        bool descent = true;
        
        psi_snsr.read();
        float pressure = psi_snsr.pressure();
        float depth = psi_snsr.depth();
        
        time_t current_time = time(NULL);
        depth_history.push_back({current_time, depth, current_velocity});

        h1.setForwards();

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
            if ((ctr%5000)==0) //every 5 seconds 
            {
                packet_t packet{current_time, pressure, depth}; //company number, time, pressure, depth
                ESP_LOGI(LOG_TAG, "%s", packet.to_string().c_str());
                data.push_back(packet);
            }

            switch (state)
            {
            case 0:
                if (ctr >= 300) //after 1 minute
                {
                    h1.setOff();
                    ++state;
                }
                break;

            case 1:
                if (ctr >= 600) //after 2 minutes
                {
                    h1.setBackwards();
                    ++state;
                }
                break;

            case 2:
                if (-0.5 <= depth || ctr >= 1200) //if at/near surface or after 4 min...
                {
                    h1.setOff();
                    goto endloop;
                }
                break;
            default:
                break;
            }
            //end of while loop
        }
        endloop:
        wifi.begin();
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
    vTaskDelete(recordTaskHandle);

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

    std::list<std::tuple<time_t, float, float>>::iterator it2; //iterate through and send all packets

    for (it2 = depth_history.begin(); it2 != depth_history.end(); it2++)
    {
        ESP_LOGI(LOG_TAG, "%s", depth_history_string(*it2).c_str());
        tcp_client.socket_send(depth_history_string(*it2));
    }

    ESP_LOGI(LOG_TAG, "Waiting for command...");
    std::string msg{};
    tcp_client.socket_receive(msg);
    ESP_LOGI(LOG_TAG, "%s", msg.c_str());

    tcp_client.socket_disconnect();
    wifi.end();

    xTaskNotify(recordTaskHandle,0,eNoAction);
   
    //xTaskCreate(record_data_task, "Data recording task", 4096, NULL, 5, &recordTaskHandle); //change priority and stack
    //xTaskCreate(test_dive_task, "Dive task", 4096, NULL, 5, NULL);

    return;
}

std::string depth_history_string(std::tuple<time_t, float, float> tup)
{
    return (std::to_string(std::get<0>(tup)) + " seconds    " + std::to_string(std::get<1>(tup)) + " meters    " + std::to_string(std::get<2>(tup)) + " m/s\n");
}
//bottom event handler

//surface event handler