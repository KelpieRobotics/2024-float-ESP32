#pragma once

#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL           /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA           /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              static_cast<i2c_port_t>(0)      /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                          /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                               /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                               /*!< I2C master doesn't need buffer */


namespace I2c
{
    class I2cController
    {
        protected:
            const i2c_port_t _ctrl_port;
            const i2c_config_t _cfg;

        public:
            constexpr I2cController() :
                _ctrl_port{I2C_MASTER_NUM},
                _cfg{
                    .mode = I2C_MODE_MASTER,
                    .sda_io_num = I2C_MASTER_SDA_IO,
                    .scl_io_num = I2C_MASTER_SCL_IO,
                    .sda_pullup_en = GPIO_PULLUP_ENABLE,
                    .scl_pullup_en = GPIO_PULLUP_ENABLE,
                    .master{I2C_MASTER_FREQ_HZ}
                }
            {

            }

            [[nodiscard]] esp_err_t init(void);
    };

    class I2cTarget
    {
        protected:
            uint8_t _dev_addr;

        public:

            constexpr I2cTarget(const uint8_t dev_addr) :
                _dev_addr{dev_addr}
            {

            }
            
            virtual esp_err_t init() =0;
            virtual esp_err_t read() =0;
            virtual esp_err_t write() =0;
    };
}

