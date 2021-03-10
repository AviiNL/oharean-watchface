#ifndef OHAREAN_H
#define OHAREAN_H

struct oharean_tm {
    int year;
    int season;
    int week;
    int day;
    int hour;
    int minute;
    int second;
};
int _server_gmt_offset();
int _server_oharean_zone();
void oharean_set_timezone(int);
double ohare_time();
struct tm oharean_epoch();
double unix2oharean(time_t);
struct oharean_tm oharean_make_date(double);
const char* oharean_season(int);

#endif
