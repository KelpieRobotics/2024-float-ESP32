#include "Adc.h"

namespace Adc
{
    [[nodiscard]] esp_err_t AdcUnit::init(void)
    {
        esp_err_t status{ESP_OK};

        status |= adc_oneshot_new_unit(&_unit_cfg, &_unit_handle);

        return status;
    }

    [[nodiscard]] esp_err_t AdcChannel::init(void)
    {
        esp_err_t status{ESP_OK};

        status |= adc_oneshot_config_channel(_adc_unit.getHandle(), _adc_channel, &_chan_cfg);

        return status;
    }
}