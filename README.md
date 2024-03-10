![](/docs/images/kelpie_logo.png)
# Kelpie Robotics - Float microcontroller
![Kelpie Robotics](https://img.shields.io/badge/Kelpie_Robotics-Underwater_Microcontroller-00a99d.svg?style=for-the-badge)

Code for the Kelpie Robotics 2024 float, written using ESP-IDF in C++.

Configurable through ESP-IDF menuconfig.

### Currently working: 
GPIO digital output

ADC reading
### Needs testing: 

### Needs implementation: 
I2C sensor class

Wifi

Leak ISR

Mission routine


## Sources:
[C++ ESP-IDF tutorial](https://www.youtube.com/watch?v=aczocIwZfZ4&list=PLowIV8ZSSsAWjoPJomVi_s1ZMghc9gNn9)
    - Using GPIO class from this tutorial series

[ESP-IDF documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/index.html)

[MS5837 driver](https://github.com/bluerobotics/BlueRobotics_MS5837_Library)
    - Arduino driver adapted for ESP-IDF