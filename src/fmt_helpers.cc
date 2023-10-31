/*
 * Created by crowx on 31/10/2023.
 *
 */

#include "fmt_helpers.h"
#include <sstream>

std::string format_uptime(int s_duration) {
    int day = s_duration / (24 * 3600);

    s_duration = s_duration % (24 * 3600);
    int hour = s_duration / 3600;

    s_duration %= 3600;
    int minutes = s_duration / 60;

    s_duration %= 60;
    int seconds = s_duration;

    std::stringstream buf;
    if (day)
        buf << day << " days, ";
    if (hour)
        buf << hour << " hours, ";
    if (minutes)
        buf << minutes << " minutes, ";
    buf << seconds << " seconds";

    return buf.str();
}
