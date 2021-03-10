#include <pebble.h>
#include "oharean.h"

int oharean_timezone = 1;

int _server_gmt_offset()
{
    time_t gmt, rawtime = time(NULL);
    struct tm *ptm;

    ptm = gmtime(&rawtime);
    // Request that mktime() looksup dst in timezone database
    ptm->tm_isdst = -1;
    gmt = mktime(ptm);

    return (int)(difftime(gmt, rawtime) / 3600);
}

int _server_oharean_zone()
{
    return oharean_timezone;
}

void oharean_set_timezone(int tz) {
    oharean_timezone = tz;
}

double ohare_time()
{
    time_t unix_time_now = time(NULL) - (_server_gmt_offset() * 3600);

    return unix2oharean(unix_time_now);
}

struct tm oharean_epoch()
{
    struct tm info;
    memset(&info, 0, sizeof(info));

    info.tm_hour = 1;
    info.tm_min = 0;
    info.tm_sec = 0;
    info.tm_mon = 6 - 1;
    info.tm_mday = 21;
    info.tm_year = 2000 - 1900;
    info.tm_isdst = -1;

    return info;
}

double unix2oharean(time_t unix_time)
{
    struct tm info = oharean_epoch();
    int oharean = mktime(&info) - _server_gmt_offset() * 3600;

    oharean -= 90 * 24 * 3600;
    // return oharean;
    int gregorian_seconds_from_epoch = (int)unix_time - oharean;

    double oharean_timestamp = gregorian_seconds_from_epoch * (2.3148148148148);

    oharean_timestamp -= 50000;

    return oharean_timestamp;
}

struct oharean_tm oharean_make_date(double time)
{
    time += 10000 * (_server_oharean_zone() * 3 - 1);

    int year = 0;
    int season = 0;
    int week = 0;
    int day = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    int days_in_year = 0;
    int seconds_in_year = 0;

    while (time >= seconds_in_year)
    {
        bool is_leap_year = false;
        if (year % 400 == 0)
            is_leap_year = true;
        else if (year % 100 == 0)
            is_leap_year = false;
        else if (year % 4 == 0)
            is_leap_year = true;

        days_in_year = is_leap_year ? 366 : 365;

        seconds_in_year = days_in_year * 200000;

        time -= seconds_in_year;

        ++year;
    }

    seconds = (int)time % 100;

    time = (time / 100);
    minutes = (int)time % 100;

    time = (time / 100);
    hours = (int)time % 20;

    time = (time / 20);
    day = (int)time % 6;

    time = (time / 6);
    week = (int)time % 15;

    time = (time / 15);
    season = (int)time;

    struct oharean_tm data;

    data.year = year;
    data.season = season;
    data.week = week;
    data.day = day;
    data.hour = hours;
    data.minute = minutes;
    data.second = seconds;

    return data;
}

const char *oharean_season(int s)
{
    if (s == 0)
        return "Ineo";
    if (s == 1)
        return "Cresco";
    if (s == 2)
        return "Vigeo";
    if (s == 3)
        return "Cado";
    return "Abeo";
}
