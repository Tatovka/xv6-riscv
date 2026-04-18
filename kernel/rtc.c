#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

static struct spinlock rtc_lock;
void init_rtc() {
    initlock(&rtc_lock, "rtc_lock");
}

uint32 rtc_high() {
    return *((volatile uint32*) RTCHIGH);
}

uint32 rtc_low() {
    return *((volatile uint32*) RTCLOW);
}

uint64 sys_get_rtc() {
    acquire(&rtc_lock);
    uint32 low = rtc_low();
    uint32 high = rtc_high();
    release(&rtc_lock);
    return ((uint64)high << 32) | low;
}