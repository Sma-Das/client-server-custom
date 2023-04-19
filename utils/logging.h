#include <time.h>

#define TIME_BUF_SIZ 20

/**
 * Returns a string representing the current time in the format "%Y-%m-%d %H:%M:%S".
 *
 * @return A string representing the current time in the format "%Y-%m-%d %H:%M:%S".
 *
 */
char *getCurrTime()
{
    static char buffer[TIME_BUF_SIZ];
    strftime(buffer, TIME_BUF_SIZ, "%Y-%m-%d %H:%M:%S", localtime(&(time_t){time(NULL)}));
    return buffer;
}
