#include "I2c.h"

namespace I2c
{
    [[nodiscard]] esp_err_t I2cController::init(void)
    {
        esp_err_t status {ESP_OK};

        status |= i2c_param_config(I2C_MASTER_NUM, &_cfg);

        status |= i2c_driver_install(I2C_MASTER_NUM, _cfg.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    }
}