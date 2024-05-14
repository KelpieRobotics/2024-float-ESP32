#pragma once
#include "ctime"
#include <string>

#define COMPANY_NUMBER static_cast<uint16_t>(CONFIG_COMPANY_NUMBER)

typedef struct {
    time_t time;
    float pressure;
    float depth;
    int company_number{COMPANY_NUMBER};

    std::string toString()
    {
        return (std::to_string(company_number) + "   " + std::to_string(time) + " seconds    " + std::to_string(pressure) + " kpa    " + std::to_string(depth) + " meters\n");
    }

} packet_t;