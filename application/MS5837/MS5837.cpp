/*
    Authors: Rustom Jehangir, Blue Robotics Inc.
             Adam Šimko, Blue Robotics Inc.

    Modified for ESP32 by Ethan Bowering @ Kelpie Robotics
    May 2024
*/

#include "MS5837.h"
#include "cmath"
#include "esp_log.h"

#define LOG_TAG "MS5837"

namespace MS5837
{    
    //const uint8_t MS5837_ADDR = 0x76;
    const uint8_t MS5837_RESET = 0x1E;
    const uint8_t MS5837_ADC_READ = 0x00;
    const uint8_t MS5837_PROM_READ = 0xA0;
    const uint8_t MS5837_CONVERT_D1_8192 = 0x4A;
    const uint8_t MS5837_CONVERT_D2_8192 = 0x5A;

    const float MS5837::Pa = 100.0f;
    const float MS5837::bar = 0.001f;
    const float MS5837::mbar = 1.0f;

    const uint8_t MS5837::MS5837_30BA = 0;
    const uint8_t MS5837::MS5837_02BA = 1;
    const uint8_t MS5837::MS5837_UNRECOGNISED = 255;

    const uint8_t MS5837_02BA01 = 0x00; // Sensor version: From MS5837_02BA datasheet Version PROM Word 0
    const uint8_t MS5837_02BA21 = 0x15; // Sensor version: From MS5837_02BA datasheet Version PROM Word 0
    const uint8_t MS5837_30BA26 = 0x1A; // Sensor version: From MS5837_30BA datasheet Version PROM Word 0


    esp_err_t MS5837::init(void) {

        esp_err_t status {ESP_OK};

        // Reset the MS5837, per datasheet
        status |= i2c_master_write_to_device(_i2c_ctrl->getPort(), _dev_addr, &MS5837_RESET, 1, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
        // Wait for reset to complete
        vTaskDelay(pdMS_TO_TICKS(10));  //if there's errors this delay is a good place to start

        // Read calibration values
        for ( uint8_t i = 0 ; i < 7 ; i++ ) {
            const uint8_t w_buf = MS5837_PROM_READ+i*2;
            uint8_t r_buf[2] = {0};
            //status |= i2c_master_write_to_device(_i2c_ctrl->getPort(), _dev_addr, &buf, sizeof(buf), pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

            //This method does a write and then a read, so we write which PROM address we want and then read it from the device
            status |= i2c_master_write_read_device(_i2c_ctrl->getPort(), _dev_addr, &w_buf, 1, r_buf, 2, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

            // This is weird. The C[i] entry is uint16_t (2 bytes) but the i2c read only takes uint8_t buffer.
            // Logic is that the first byte will be in r_buf[0], so shift it by 8 to the MSB and or it with the next byte read.
            C[i] = r_buf[0];
            C[i] = (C[i] << 8) | r_buf[1];
        }

        // Verify that data is correct with CRC
        uint8_t crcRead = C[0] >> 12;
        uint8_t crcCalculated = crc4(C);

        if ( crcCalculated != crcRead ) {
            ESP_LOGI(LOG_TAG, "CRC FAIL");
            return (status |= ESP_FAIL);
        }

        ESP_LOGD(LOG_TAG, "CRC OK!");

        uint8_t version = (C[0] >> 5) & 0x7F; // Extract the sensor version from PROM Word 0

        // Set _model according to the sensor version
        if (version == MS5837_02BA01)
        {
            _model = MS5837_02BA;
        }
        else if (version == MS5837_02BA21)
        {
            _model = MS5837_02BA;
        }
        else if (version == MS5837_30BA26)
        {
            _model = MS5837_30BA;
        }
        else
        {
            _model = MS5837_UNRECOGNISED;
        }
        // The sensor has passed the CRC check, so we should return true even if
        // the sensor version is unrecognised.
        // (The MS5637 has the same address as the MS5837 and will also pass the CRC check)
        // (but will hopefully be unrecognised.)
        ESP_LOGD(LOG_TAG, "Model: %d", _model);
        return status;
    }

    void MS5837::setModel(uint8_t model) {
        _model = model;
    }

    uint8_t MS5837::getModel() {
        return (_model);
    }

    void MS5837::setFluidDensity(float density) {
        fluidDensity = density;
    }

    esp_err_t MS5837::read() {

        esp_err_t status{ESP_OK};

        // Request D1 conversion
        status |= i2c_master_write_to_device(_i2c_ctrl->getPort(), _dev_addr, &MS5837_CONVERT_D1_8192, 1, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

        vTaskDelay(pdMS_TO_TICKS(30));  // Max conversion time per datasheet is 20ms, but for some reason this wasn't enough here

        //request ADC read
        status |= i2c_master_write_to_device(_i2c_ctrl->getPort(), _dev_addr, &MS5837_ADC_READ, 1, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

        //get values read
        uint8_t r_buf[3] = {0};
        i2c_master_read_from_device(_i2c_ctrl->getPort(), _dev_addr, r_buf, 3, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

        //build final value up from resulting 3 bytes
        D1_pres = 0;
        D1_pres = r_buf[0];
        D1_pres = (D1_pres << 8) | r_buf[1];
        D1_pres = (D1_pres << 8) | r_buf[2];

        //request D2 conversion
        status |= i2c_master_write_to_device(_i2c_ctrl->getPort(), _dev_addr, &MS5837_CONVERT_D2_8192, 1, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

        vTaskDelay(pdMS_TO_TICKS(30));  // Max conversion time per datasheet is 20ms, but for some reason this wasn't enough here

        //request ADC read
        status |= i2c_master_write_to_device(_i2c_ctrl->getPort(), _dev_addr, &MS5837_ADC_READ, 1, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

        //get values read
        r_buf[0] = r_buf[1] = r_buf[2] = 0;
        i2c_master_read_from_device(_i2c_ctrl->getPort(), _dev_addr, r_buf, 3, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

        //_i2cPort->requestFrom(MS5837_ADDR,3);
        D2_temp = 0;
        D2_temp = r_buf[0];
        D2_temp = (D2_temp << 8) | r_buf[1];
        D2_temp = (D2_temp << 8) | r_buf[2];

        calculate();

        return status;
    }

    void MS5837::calculate() {
        // Given C1-C6 and D1, D2, calculated TEMP and P
        // Do conversion first and then second order temp compensation

        int32_t dT = 0;
        int64_t SENS = 0;
        int64_t OFF = 0;
        int32_t SENSi = 0;
        int32_t OFFi = 0;
        int32_t Ti = 0;
        int64_t OFF2 = 0;
        int64_t SENS2 = 0;

        // Terms called
        dT = D2_temp-uint32_t(C[5])*256l;
        if ( _model == MS5837_02BA ) {
            SENS = int64_t(C[1])*65536l+(int64_t(C[3])*dT)/128l;
            OFF = int64_t(C[2])*131072l+(int64_t(C[4])*dT)/64l;
            P = (D1_pres*SENS/(2097152l)-OFF)/(32768l);
        } else {
            SENS = int64_t(C[1])*32768l+(int64_t(C[3])*dT)/256l;
            OFF = int64_t(C[2])*65536l+(int64_t(C[4])*dT)/128l;
            P = (D1_pres*SENS/(2097152l)-OFF)/(8192l);
        }

        // Temp conversion
        TEMP = 2000l+int64_t(dT)*C[6]/8388608LL;

        //Second order compensation
        if ( _model == MS5837_02BA ) {
            if((TEMP/100)<20){         //Low temp
                Ti = (11*int64_t(dT)*int64_t(dT))/(34359738368LL);
                OFFi = (31*(TEMP-2000)*(TEMP-2000))/8;
                SENSi = (63*(TEMP-2000)*(TEMP-2000))/32;
            }
        } else {
            if((TEMP/100)<20){         //Low temp
                Ti = (3*int64_t(dT)*int64_t(dT))/(8589934592LL);
                OFFi = (3*(TEMP-2000)*(TEMP-2000))/2;
                SENSi = (5*(TEMP-2000)*(TEMP-2000))/8;
                if((TEMP/100)<-15){    //Very low temp
                    OFFi = OFFi+7*(TEMP+1500l)*(TEMP+1500l);
                    SENSi = SENSi+4*(TEMP+1500l)*(TEMP+1500l);
                }
            }
            else if((TEMP/100)>=20){    //High temp
                Ti = 2*(dT*dT)/(137438953472LL);
                OFFi = (1*(TEMP-2000)*(TEMP-2000))/16;
                SENSi = 0;
            }
        }

        OFF2 = OFF-OFFi;           //Calculate pressure and temp second order
        SENS2 = SENS-SENSi;

        TEMP = (TEMP-Ti);

        if ( _model == MS5837_02BA ) {
            P = (((D1_pres*SENS2)/2097152l-OFF2)/32768l);
        } else {
            P = (((D1_pres*SENS2)/2097152l-OFF2)/8192l);
        }
    }

    float MS5837::pressure(float conversion) {
        if ( _model == MS5837_02BA ) {
            return P*conversion/100.0f;
        }
        else {
            return P*conversion/10.0f;
        }
    }

    float MS5837::temperature() {
        return TEMP/100.0f;
    }

    // The pressure sensor measures absolute pressure, so it will measure the atmospheric pressure + water pressure
    // We subtract the atmospheric pressure to calculate the depth with only the water pressure
    // The average atmospheric pressure of 101300 pascal is used for the calcuation, but atmospheric pressure varies
    // If the atmospheric pressure is not 101300 at the time of reading, the depth reported will be offset
    // In order to calculate the correct depth, the actual atmospheric pressure should be measured once in air, and
    // that value should subtracted for subsequent depth calculations.
    float MS5837::depth() {
        return (pressure(MS5837::Pa)-101300)/(fluidDensity*9.80665);
    }

    float MS5837::altitude() {
        return (1-pow((pressure()/1013.25),.190284))*145366.45*.3048;
    }


    uint8_t MS5837::crc4(uint16_t n_prom[]) {
        uint16_t n_rem = 0;

        n_prom[0] = ((n_prom[0]) & 0x0FFF);
        n_prom[7] = 0;

        for ( uint8_t i = 0 ; i < 16; i++ ) {
            if ( i%2 == 1 ) {
                n_rem ^= (uint16_t)((n_prom[i>>1]) & 0x00FF);
            } else {
                n_rem ^= (uint16_t)(n_prom[i>>1] >> 8);
            }
            for ( uint8_t n_bit = 8 ; n_bit > 0 ; n_bit-- ) {
                if ( n_rem & 0x8000 ) {
                    n_rem = (n_rem << 1) ^ 0x3000;
                } else {
                    n_rem = (n_rem << 1);
                }
            }
        }

        n_rem = ((n_rem >> 12) & 0x000F);

        return n_rem ^ 0x00;
    }
}