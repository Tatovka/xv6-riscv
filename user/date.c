#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define BILLION 1000000000 

#define SECONDS(ut) (ut / BILLION)
#define MINUTES(ut) (SECONDS(ut) / 60)
#define HOURS(ut) (MINUTES(ut) / 60)
#define DAYS(ut) (HOURS(ut) / 24)

#define NANOSECOND(ut) ((ut % BILLION + BILLION) % BILLION)

struct date {
    long year;
    int month;
    int day;
};

struct date date_from_unix_time(long ut) {
    const static int month_length[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    long days = DAYS(ut);
    long hour = HOURS(ut);
    days -= (hour % 24 < 0); // rounding to day start
    struct date res;
    #define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
    #define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))
    #define IS_LEAP(y) (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))

    long y = 1970;
    while (days < 0 || days >= (IS_LEAP (y) ? 366 : 365))
    {
      /* Guess a corrected year, assuming 365 days per year.  */
      long yg = y + days / 365 - (days % 365 < 0);
      /* Adjust DAYS and Y to match the guessed year.  */
      days -= ((yg - y) * 365
	       + LEAPS_THRU_END_OF (yg - 1)
	       - LEAPS_THRU_END_OF (y - 1));
      y = yg;
    }
    res.year = y;
    res.day = days;
    
    int dayInYear = days;
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
    #define SECS_IN_DAY (3600 * 24)
    int seconds = SECONDS(ut) % SECS_IN_DAY;
    seconds += SECS_IN_DAY * (seconds < 0);
    int minutes = seconds / 60;
    int hours = minutes / 60;
    print_dd(hours);
    printf(":");
    print_dd(minutes % 60);
    printf(":");
    print_dd(seconds % 60);
    printf(".%ld\n", NANOSECOND(ut));
}

int main() {
    long unixTime = get_rtc();
    print_date(unixTime);
    return 0;
}