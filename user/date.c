#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define SECONDS(ut) (ut / 1000000000)
#define MINUTES(ut) (SECONDS(ut) / 60)
#define HOURS(ut) (MINUTES(ut) / 60)
#define DAYS(ut) (HOURS(ut) / 24)

#define HOUR(ut) (HOURS(ut) % 24)
#define MINUTE(ut) (MINUTES(ut) % 60)
#define SECOND(ut) (SECONDS(ut) % 60)
#define NANOSECOND(ut) (ut % 1000000000)


long years_from_epoch(long ut) {
    long days = DAYS(ut);
    const long DAYS_IN_CYCLE = 365 * 4 + 1;
    long cycle = days / DAYS_IN_CYCLE;
    long cycleStart = cycle * 4;
    long dayInCycle = days % DAYS_IN_CYCLE;
    dayInCycle -= dayInCycle >= 1095;
    return dayInCycle / 365 + cycleStart;
}

struct date {
    long year;
    int month;
    int day;
};

struct date date_from_unix_time(long ut) {
    const static int month_length[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    long days = DAYS(ut);
    long hour = HOUR(ut);
    if (days < 0) days -= hour != 0; // rounding to day start
    const long DAYS_IN_CYCLE = 365 * 4 + 1;
    long cycle = days / DAYS_IN_CYCLE - (ut < 0);

    long cycleStart = cycle * 4;
    long cycleDayStart = cycle * DAYS_IN_CYCLE;
    int dayOfCycle = days - cycleDayStart; 
    long yearOfCycle = (dayOfCycle - (dayOfCycle == 1095)) / 365;
    int dayInYear = dayOfCycle - 365 * yearOfCycle - (yearOfCycle == 3);

    struct date res;
    res.year = yearOfCycle + cycleStart + 1970;
    if (yearOfCycle == 2 && dayInYear >= 59) { //leap year
        if (dayInYear == 59) { //29 лют 
            res.day = 28;
            res.month = 1;
            return res;
        }
        dayInYear -= 1;
    }
    for (int m = 0; m < 12; ++m) {
        if (dayInYear < month_length[m]) {
            res.month = m;
            res.day = dayInYear;
            return res;
        }
        dayInYear -= month_length[m];
    }
    fprintf(2, "wrong day\n");
    exit(-1);
}

void print_dd(uint d) {
    if (d < 10) printf("0%d", d);
    else printf("%d", d);
}

void print_date(long ut) {
    const static char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    struct date dt = date_from_unix_time(ut);
    printf("%ld %s ", dt.year, months[dt.month]);
    print_dd(dt.day + 1);
    printf(" ");
    print_dd(HOUR(ut));
    printf(":");
    print_dd(MINUTE(ut));
    printf(":");
    print_dd(SECOND(ut));
    printf(".%ld\n", NANOSECOND(ut));
}

int main() {
    long unixTime = get_rtc();
    print_date(unixTime);
    return 0;
}