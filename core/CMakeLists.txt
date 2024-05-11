set(SOURCES ./Src/Selkie.cpp)

idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS . Inc
                        REQUIRES 
                            freertos 
                            esp_common 
                            Gpio
                            Adc
                            Hbridge
                            I2c
                            MS5837
                            Wifi
                            nvs_flash
                            esp_event)