#pragma once

#include "esp_adc/adc_oneshot.h"

namespace Adc
{
    class Adc
    {
        const adc_oneshot_unit_handle_t _unit_handle;
        const adc_oneshot_unit_init_cfg_t _unit_cfg;

        public:

        Adc(const adc_oneshot_unit_handle_t unit_handle, const adc_oneshot_unit_init_cfg_t unit_cfg) :
            _unit_handle{unit_handle},
            _unit_cfg{unit_cfg}
        {
            
        }

        [[nodiscard]] esp_err_t init(void);
    };

    class AdcChannel
    {

    };

}
