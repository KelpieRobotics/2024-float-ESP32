#pragma once

#include "driver/gpio.h"

namespace Gpio
{
    class GpioBase
    {
        protected:
            const gpio_num_t _pin;
            const gpio_config_t _cfg;
            const bool _invert_logic = false;

        public: 
            //gpio_num_t pin(void) const {return _pin;}

            constexpr GpioBase(const gpio_num_t pin, 
                        const gpio_config_t& cfg, //should this be a reference? 
                        const bool invert_logic = false) :
                _pin{pin}, 
                _cfg{cfg},
                _invert_logic{invert_logic}
            {

            }

            virtual bool state(void) =0; //abstract, must be defined in derived classes
            virtual esp_err_t set(const bool state) =0;

            [[nodiscard]] esp_err_t init(void); //nodiscard is a programming guard, is checked at compile time. 
                                                //When we init a pin we NEED its value, if we don't take it then there is an error


    };

    class GpioOutput : public GpioBase
    {
        bool _state = false;

        public: 
            constexpr GpioOutput(const gpio_num_t pin, const bool invert_logic = false) : 
                GpioBase{pin, 
                            gpio_config_t{
                                .pin_bit_mask   = static_cast<uint64_t>(1) << pin,
                                .mode           = GPIO_MODE_OUTPUT,
                                .pull_up_en     = GPIO_PULLUP_DISABLE,
                                .pull_down_en   = GPIO_PULLDOWN_ENABLE,
                                .intr_type      = GPIO_INTR_DISABLE
                            },
                            invert_logic}
            {

            }

            [[nodiscard]] esp_err_t init(void);

            esp_err_t set(const bool state);
            //esp_err_t toggle(void);
            bool state(void) {return _state;};
    };
/*
    class GpioInput

        gpio_pin_t _pin;
    {
        public:
            esp_err_t init(void);
            bool state(void);
    }
    
*/
} // namespace Gpio
