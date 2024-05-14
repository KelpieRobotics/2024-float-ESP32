/*
    Ethan Bowering @ Kelpie Robotics
    May 2024
*/

#include "I2c.h"

namespace I2c
{
    [[nodiscard]] esp_err_t I2cController::init(void)
    {
        esp_err_t status {ESP_OK};

        status |= i2c_param_config(_ctrl_port, &_cfg);

        status |= i2c_driver_install(_ctrl_port, _cfg.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

        return status;
    }

    i2c_port_t I2cController::getPort(void)
    {
        return _ctrl_port;
    }
}