#pragma once
#include "ctime"

typedef struct {
    int companyNumber;
    time_t time;
    float pressure;
    float depth;

} packet_t;