#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Hbridge.h"
#include "Adc.h"
#include "I2c.h"
#include "MS5837.h"
#include "Wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mutex"
#include "packet.h"

#include <list>

#define pdSECOND pdMS_TO_TICKS(1000)

#define HBRIDGE_PIN_1 static_cast<gpio_num_t>(CONFIG_HBRIDGE_PIN_1)
#define HBRIDGE_PIN_2 static_cast<gpio_num_t>(CONFIG_HBRIDGE_PIN_2)

#define ADC_UNIT static_cast<adc_unit_t>(CONFIG_ADC_UNIT)

#define LEAK_SENS_PIN static_cast<adc_channel_t>(CONFIG_LEAK_SENS_PIN)
#define PRESSURE_SENS_ADDR static_cast<uint8_t>(CONFIG_PRESSURE_SENS_ADDR)

/*class Selkie final

{
    public:*/

        enum class state_e //state of float
        {
            NOT_INITIALISED,
            INITIALISED,
            WIFI_CONNECTING,
            WIFI_CONNECTED,
            SOCKET_CONNECTING,
            SOCKET_CONNECTED,
            SENDING_DATA,
            AWAITING_COMMAND,
            DIVING,
            SURFACING,
            MISSION_COMPLETE,
            LEAK_DETECTED,
            ERROR
        };

        static state_e _state{state_e::NOT_INITIALISED};
        static std::mutex state_mutx;
        static std::mutex data_mutx;
        static std::list<packet_t> data{};

        esp_err_t setup(void);
        void loop(void);

        Hbridge::Hbridge h1 {HBRIDGE_PIN_1, HBRIDGE_PIN_2}; 

        //Adc::AdcUnit adc_unit {ADC_UNIT};

        //Adc::AdcChannel pressure_sens {&adc_unit, PRESSURE_SENS_PIN};

        I2c::I2cController i2c_ctrl {};

        MS5837::MS5837 psi_snsr {PRESSURE_SENS_ADDR, &i2c_ctrl};

        float pressure{0};
        
        float temp{0};

        float depth{0};

        float altitude{0};

        void record_data(void);

        WIFI::Wifi wifi;

        esp_err_t wifi_connect(void);

//};