#pragma once

#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASS CONFIG_WIFI_PASS
#define HOSTNAME CONFIG_HOSTNAME

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"


#include <algorithm>
#include <mutex>

#include <cstring>

namespace WIFI
{

class Wifi
{
    constexpr static const char* _log_tag{"WiFi"};

    constexpr static const char* ssid{WIFI_SSID};
    constexpr static const char* password{WIFI_PASS};

public:
    // Strongly typed enum to define our state
    enum class state_e
    {
        NOT_INITIALISED,
        INITIALISED,
        READY_TO_CONNECT,
        CONNECTING,
        WAITING_FOR_IP,
        CONNECTED,
        DISCONNECTED,
        ERROR
    };

    // "Rule of 5" Constructors and assignment operators
    // Ref: https://en.cppreference.com/w/cpp/language/rule_of_three
    Wifi(void);
    ~Wifi(void)                     = default;
    Wifi(const Wifi&)               = default;
    Wifi(Wifi&&)                    = default;
    Wifi& operator=(const Wifi&)    = default;
    Wifi& operator=(Wifi&&)         = default;

    esp_err_t init(void);

    /** Connect to AP
    */
    esp_err_t begin(void);

    /** Disconnect from AP
    */
    esp_err_t end(void);

    constexpr const state_e& get_state(void) { return _state; }

    constexpr static const char* get_mac(void) 
        { return mac_addr_cstr; }

private:
    static esp_err_t _init(void);
    static wifi_init_config_t wifi_init_config;
    static wifi_config_t wifi_config;

    void state_machine(void);   // TODO static

    static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
    static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);
    static void ip_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);

    static state_e _state;

    // Get the MAC from the API and convert to ASCII HEX
    static esp_err_t _get_mac(void);

    static char mac_addr_cstr[13];      ///< Buffer to hold MAC as cstring
    static std::mutex init_mutx;        ///< Initialisation mutex
    static std::mutex connect_mutx;     ///< Connect mutex
    static std::mutex disconnect_mutx;  ///< Disconnect mutex
    static std::mutex state_mutx;       ///< State change mutex
};


} // namespace WIFI