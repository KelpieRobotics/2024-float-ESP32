#include "Hbridge.h"
#include "Gpio.h"
#include "esp_log.h"
#define LOG_TAG "Hbridge"

namespace Hbridge
{
    [[nodiscard]] esp_err_t Hbridge::init(void)
    {
        esp_err_t status{ESP_OK};

        status |= _pin1.init();
        status |= _pin2.init();

        return status;
    }

    int Hbridge::getState(void)
    {
        return(_state);
    }

    esp_err_t Hbridge::setOff(void)
    {
        esp_err_t status{ESP_OK};

        status |= _pin1.set(false);
        status |= _pin2.set(false);

        _state = 0;

        ESP_LOGD(LOG_TAG, "Hbridge OFF");

        return status;
    }

    esp_err_t Hbridge::setForwards(void)
    {
        esp_err_t status{ESP_OK};

        status |= _pin1.set(true);
        status |= _pin2.set(false);

        _state = 1;

        ESP_LOGD(LOG_TAG, "Hbridge FORWARDS");

        return status;
    }

    esp_err_t Hbridge::setBackwards(void)
    {
        esp_err_t status{ESP_OK};

        status |= _pin1.set(false);
        status |= _pin2.set(true);

        _state = 2;

        ESP_LOGD(LOG_TAG, "Hbridge BACKWARDS");

        return status;
    }

}