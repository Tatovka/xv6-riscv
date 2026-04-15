#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define SECONDS(ut) (ut / 1000000000)
#define MINUTES(ut) (SECONDS(ut) / 60)
#define HOURS(ut) (MINUTES(ut) / 60)
#define DAYS(ut) (HOURS(ut) / 24)

#define YEARS(ut) (DAYS(ut) / 365)
#define YEAR(ut) (1970 + YEARS(ut))



int main() {
    uint64 unixTime = get_rtc();
    printf("%lu\n", YEAR(unixTime));
    return 0;
}