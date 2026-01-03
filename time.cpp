#include <chrono>
#include <ctime>

extern "C" {
#include "main.h"
#include <sys/time.h>
}

extern "C"
int _getentropy(uint8_t*, int) {
	return 0;
}

extern "C"
int _gettimeofday(struct timeval *tv, void *tzvp) {
	struct timezone *tzp {static_cast<struct timezone *>(tzvp)};
	if (tv) {
		extern RTC_HandleTypeDef hrtc;
	    /* Ask the host for the seconds since the Unix epoch.  */
	    RTC_DateTypeDef rtcDate {};
	    RTC_TimeTypeDef rtcTime {};
	    HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
	    HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
	    struct tm c_time{};
	    c_time.tm_year = 125 + rtcDate.Year;  // since 2025
	    c_time.tm_mon = rtcDate.Month - 1;  // tm_mon[0, 11] <?> Month[1, 12]
	    c_time.tm_mday = rtcDate.Date;  // tm_mday[1, 31] <?> Date[1, 31]
	    c_time.tm_wday = rtcDate.WeekDay - 1;  // tm_wday[0, 6] <?> WeekDay[1, 7]
	    c_time.tm_hour = rtcTime.Hours;  // tm_hour[0, 23] <?> Hours[0, 23]
	    c_time.tm_min = rtcTime.Minutes;  // tm_min[0, 59] <?> Minutes[0, 59]
	    c_time.tm_sec = rtcTime.Seconds;  // tm_sec[0, 60] <?> Seconds[0, 59]
	    tv->tv_sec = mktime(&c_time);
	    tv->tv_usec = ((rtcTime.SecondFraction - rtcTime.SubSeconds) * 1000000) / (rtcTime.SecondFraction+1);
	}

	/* Return fixed data for the timezone.  */
	if (tzp) {
	    tzp->tz_minuteswest = 0;
	    tzp->tz_dsttime = 0;
	}
	return 0;
}
