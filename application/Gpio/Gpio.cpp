/*
    Simon Howroyd
    Modified by Ethan Bowering @ Kelpie Robotics
    May 2024
*/

#include "Gpio.h"

namespace Gpio
{
    [[nodiscard]] esp_err_t GpioBase::init(void)
    {
        esp_err_t status{ESP_OK};

        status |= gpio_config(&_cfg);

        return status;
    }

    [[nodiscard]] esp_err_t GpioOutput::init(void)
    {
        esp_err_t status{GpioBase::init()};

        if (ESP_OK == status)
        {
            status |= set(_invert_logic);
        }

        return status;
    }

    esp_err_t GpioOutput::set(const bool state)
    {
        _state = state;
        
        return gpio_set_level(_pin, _invert_logic ? !state : state);
    }
}