#include "types.h"
#include "memlayout.h"

uint32 rtc_high() {
    return *((volatile uint32*) RTCHIGH);
}

uint32 rtc_low() {
    return *((volatile uint32*) RTCLOW);
}

uint64 sys_get_rtc() {
    uint32 low = rtc_low();
    uint32 high = rtc_high();
    return ((uint64)high << 32) | low;
}