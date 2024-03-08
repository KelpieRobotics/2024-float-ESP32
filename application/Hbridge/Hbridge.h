#pragma once
#include "Gpio.h"

namespace Hbridge
{
    class Hbridge
    {
        protected: 
            Gpio::GpioOutput _pin1;
            Gpio::GpioOutput _pin2;

            int _state; //returns 0 if 00, 1 if 01, 2 if 10

        public:
            constexpr Hbridge(const gpio_num_t pin1, const gpio_num_t pin2) :
                _pin1{pin1},
                _pin2{pin2},
                _state{0}
                {
                    
                }

            [[nodiscard]] esp_err_t init(void);

            int getState(void);
            esp_err_t setOff(void);
            esp_err_t setForwards(void);
            esp_err_t setBackwards(void);

    };
}