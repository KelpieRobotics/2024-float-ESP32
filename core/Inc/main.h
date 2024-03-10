#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Hbridge.h"
#include "Adc.h"

#define pdSECOND pdMS_TO_TICKS(1000)

#define HBRIDGE_PIN_1 static_cast<gpio_num_t>(CONFIG_HBRIDGE_PIN_1)
#define HBRIDGE_PIN_2 static_cast<gpio_num_t>(CONFIG_HBRIDGE_PIN_2)

#define ADC_UNIT static_cast<adc_unit_t>(CONFIG_ADC_UNIT)

#define LEAK_SENS_PIN static_cast<adc_channel_t>(CONFIG_LEAK_SENS_PIN)
#define PRESSURE_SENS_PIN static_cast<adc_channel_t>(CONFIG_PRESSURE_SENS_PIN)

class Main final

{
    public:
        esp_err_t setup(void);
        void loop(void);

        Hbridge::Hbridge h1 {HBRIDGE_PIN_1, HBRIDGE_PIN_2}; 

        Adc::AdcUnit adc_unit {ADC_UNIT};

        Adc::AdcChannel pressure_sens {&adc_unit, PRESSURE_SENS_PIN};

        int pressure;

};