#include <errno.h>
#include <string.h>

#undef strerror

char *
strerror(int errnum)
{
	if (errnum > EUNKNOWN || errnum <= 0)
		errnum = EUNKNOWN;
	return _sys_errlist[errnum];
}
