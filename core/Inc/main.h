#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Hbridge.h"
#define pdSECOND pdMS_TO_TICKS(1000)

#define HBRIDGE_PIN_1 CONFIG_HBRIDGE_PIN_1
#define HBRIDGE_PIN_2 CONFIG_HBRIDGE_PIN_2

class Main final
{
    public:
        esp_err_t setup(void);
        void loop(void);

    Hbridge::Hbridge h1 {static_cast<gpio_num_t>(HBRIDGE_PIN_1), static_cast<gpio_num_t>(HBRIDGE_PIN_2)}; 
};