#pragma once

#include "esp_adc/adc_oneshot.h"

#define ADC_ATTEN static_cast<adc_atten_t>(CONFIG_ADC_ATTEN)

namespace Adc
{
    /**
     * Class built around the oneshot driver.
     * For our project we should only use ADC1 because ADC2 is in use by the wifi driver, however if needed adc_oneshot_read() is thread safe.
    */
    class AdcUnit
    {
        protected:
            adc_oneshot_unit_handle_t _unit_handle; 
            const adc_oneshot_unit_init_cfg_t _unit_cfg;

        public:

            constexpr AdcUnit(adc_unit_t unit) : 
                _unit_handle{},
                _unit_cfg{
                    .unit_id = unit,
                    .clk_src = static_cast<adc_oneshot_clk_src_t>(0), //default clock source
                    .ulp_mode = ADC_ULP_MODE_DISABLE
                }
            {
                
            }

            adc_oneshot_unit_handle_t getHandle(void);
            adc_oneshot_unit_init_cfg_t getConfig(void);

            [[nodiscard]] esp_err_t init(void);
    };

    class AdcChannel
    {
        AdcUnit _adc_unit;
        adc_channel_t _adc_channel;
        const adc_oneshot_chan_cfg_t _chan_cfg;

        constexpr AdcChannel(AdcUnit adc_unit, adc_channel_t adc_channel) : //change this to create a config like gpio by passing parameters
            _adc_unit{adc_unit},
            _adc_channel{adc_channel},
            _chan_cfg{
                .atten = ADC_ATTEN,
                .bitwidth = ADC_BITWIDTH_DEFAULT
            }
            {

            }

        [[nodiscard]] esp_err_t init(void);

        esp_err_t read(int *readValue);
    };

}
