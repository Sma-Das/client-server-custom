#include <time.h>

#define TIME_BUF_SIZ 20

char *getCurrTime()
{
    static char buffer[TIME_BUF_SIZ];
    strftime(buffer, TIME_BUF_SIZ, "%Y-%m-%d %H:%M:%S", localtime(&(time_t){time(NULL)}));
    return buffer;
}
