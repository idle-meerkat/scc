#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "../libc.h"

#undef strtoul

unsigned long
strtoul(const char * restrict ptr, char ** restrict end, int base)
{
	int d, sign = 1;
	unsigned long n;
	char *t, *s = (char *) ptr;

	if (end)
		*end = s;

	while (isspace(*s))
		++s;

	switch (*s) {
	case '-':
		sign = -1;
	case '+':
		++s;
	}

	if (base == 0) {
		if (*s == '0')
			base = toupper(s[1]) == 'X' ? 16 : 8;
		else
			base = 10;
	}
	if (base == 16 && *s == '0' && toupper(s[1]) == 'X')
		s += 2;

	n = 0;
	for (t = s; (d = _dtoi(*t)) < base; ++t) {
		if (n > ULONG_MAX/base)
			goto overflow;
		n *= base;
		if (d > ULONG_MAX - n)
			goto overflow;
		n += d;
	}

	if (end && t != s)
		*end = t;

	return n*sign;

overflow:
	if (end)
		*end = t;
	errno = ERANGE;

	return ULONG_MAX;
}
