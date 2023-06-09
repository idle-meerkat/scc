#ifndef _TIME_H
#define _TIME_H

#define _NEED_SIZET
#define _NEED_NULL
#include <sys/cdefs.h>
#include <arch/cdefs.h>
#include <arch/time.h>

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;

	/* fields used internally */

	char *tm_zone;
	long tm_gmtoff;
};

extern clock_t clock(void);
extern double difftime(time_t, time_t);
extern time_t mktime(struct tm *);
extern time_t time(time_t *);
extern char *asctime(const struct tm *);
extern char *ctime(const time_t *);
extern struct tm *gmtime(const time_t *);
extern struct tm *localtime(const time_t *);
extern size_t strftime(char *restrict, size_t, const char *restrict,
                       const struct tm *restrict);

#endif
